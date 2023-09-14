#include "TaskSerial.h"

using namespace TaskMiddleWare;

TaskSerial::TaskSerial() {
    is_loadDevList = true;
    is_loadSerialList = true;
    is_mergeDevAndSerial = true;
    is_RecvRun = true;
    isStopThread = false;

    pointCount = 0;

    devList.clear();
    serialList.clear();
    serialInfoMap.clear();
    protocolInfoMap.clear();

    pscProtocolPath = "data/csi_pcs_modbus.json";
    loadPcsProtocolByJson(pscProtocolPath);

    MessageQueue<std::vector<DevInfo>>::getInstance().subscribe("Setting/DevList", [&](const std::vector<DevInfo>& msg) {
        std::unique_lock<std::mutex> list_lck(list_mutex);
        // 处理订阅到的消息
        if (is_loadDevList == false) {
            if (devList.size() < msg.size()) { // 新增
                for (auto dev : msg) {
                    auto it = std::find_if(devList.begin(), devList.end(), [&dev](const DevInfo& v) {
                        return v.devName == dev.devName;  // 自定义比较逻辑
                    });

                    if (it == devList.end()) {
                        for (auto s_it = serialInfoMap.begin(); s_it != serialInfoMap.end(); s_it++) {
                            if (s_it->first == dev.serialInfo.name) {
                                if ("modbus" == dev.protocol) {
                                    protocolInfoMap[dev.devName] = std::make_shared<Modbus>(s_it->second.get());
                                }
                            }
                            // 这里最好加一个未匹配到串口名的错误检测
                        }

                        break;
                    }
                }  
            } else if (devList.size() > msg.size()) { // 删除
                for ( auto dev = devList.begin(); dev != devList.end(); ) {
                    int match = 0;
                    for (auto it : msg) {
                        if (it.devName == dev->devName) {
                            match = 1;
                            break;
                        }
                    }

                    if (!match) {
                        devList.erase(dev);

                        LOG_DEBUG("destruct protocol list success\n");
                    } else {
                        dev++;
                    }
                }

                for ( auto protocol = protocolInfoMap.begin(); protocol != protocolInfoMap.end(); ) {
                    int match = 0;
                    for (auto it : msg) {
                        if (it.devName == protocol->first) {
                            match = 1;
                            break;
                        }
                    }

                    if (!match) {
                        protocolInfoMap.erase(protocol);

                        LOG_DEBUG("destruct protocol success\n");
                    } else {
                        protocol++;
                    }
                }
            }
        } else {
            is_loadDevList = false;
        }

        devList = msg;
    });

    MessageQueue<std::vector<SerialIdInfo>>::getInstance().subscribe("Setting/serialList", [&](const std::vector<SerialIdInfo>& msg) {
        std::unique_lock<std::mutex> list_lck(list_mutex);
        // 处理订阅到的消息
        if (is_loadSerialList == false) {
            if (serialList.size() < msg.size()) { // 新增
                for (auto serial : msg) {
                    auto it = std::find_if(serialList.begin(), serialList.end(), [&serial](const SerialIdInfo& s) {
                        return s.name == serial.name;  // 自定义比较逻辑
                    });

                    if (it == serialList.end()) {
                        // 判断是不是485串口
                        if (serial.SerialName == "RS485-1" || 
                            serial.SerialName == "RS485-2" ||
                            serial.SerialName == "RS485-3" ||
                            serial.SerialName == "RS485-4" ||
                            serial.SerialName == "RS485-5" ||
                            serial.SerialName == "RS485-6" ||
                            serial.SerialName == "RS485-7" ||
                            serial.SerialName == "RS485-8" ) 
                        {
                            // 创建一个485串口
                            serialInfoMap[serial.name] = std::make_shared<Serial_485>(serial);
                            LOG_DEBUG("create serial_485 success {}/{}/{}/{}/{}/{}/{}/{}/{}\n", 
                                        serial.name, serial.SerialName, serial.SerialId, 
                                        serial.serialParamInfo.baudrate, 
                                        serial.serialParamInfo.databit,
                                        serial.serialParamInfo.parity,
                                        serial.serialParamInfo.stopbit,
                                        serial.serialParamInfo.vmin,
                                        serial.serialParamInfo.vtime);
                        }

                        break;
                    }
                }
            } else if (serialList.size() > msg.size()) { // 删除
                for ( auto serial = serialList.begin(); serial != serialList.end(); ) {
                    int match = 0;
                    for (auto it : msg) {
                        if (it.name == serial->name) {
                            match = 1;
                            break;
                        }
                    }

                    if (!match) {
                        serialList.erase(serial);
                        LOG_DEBUG("destruct serial_485 list success\n");
                    } else {
                        serial++;
                    }
                }

                for ( auto serial = serialInfoMap.begin(); serial != serialInfoMap.end(); ) {
                    int match = 0;
                    for (auto it : msg) {
                        if (it.SerialName == serial->first) {
                            match = 1;
                            break;
                        }
                    }

                    if (!match) {
                        serialInfoMap.erase(serial);

                        LOG_DEBUG("destruct serial_485 success\n");
                    } else {
                        serial++;
                    }
                }
            }
        } else {
            is_loadSerialList = false;
        }

        serialList = msg;
    });

    MessageQueue<std::vector<modbusPoint>>::getInstance().subscribe("modbus/point", [&](const std::vector<modbusPoint>& msg) {
        LOG_DEBUG("recv point len:%d\n", msg.size());
        v_point = msg;
    });

    recv_thread = std::thread([&]() {
        while(true) {
            // 这个信号量等待主线程先启动
            {
                while (true == is_RecvRun) {
                    std::unique_lock<std::mutex> wait_lock(recv_wait_mutex);
                    recv_cv.wait(wait_lock);
                }

                uint8_t* recv_buf = new uint8_t[RECVBUF];
                int recv_len = 0;
                std::weak_ptr<Protocol> recv_pit = pit;

                if (auto sharedPtr = recv_pit.lock()) {
                    // recv_len = sharedPtr->receive((char*)recv_buf);
                    recv_len = sharedPtr->receive(recv_buf);
                } else {
                    LOG_ERROR("The object has been destroyed.\n");
                }

                {
                    std::unique_lock<std::mutex> lock(recv_mutex);
                    LOG_DEBUG("recv_len:{}\n", recv_len);

                    // 将收数据的内存置空，才重新分配固定大小的内存
                    if (recv_len == -1) {
                        total_recv_buf.reserve(0);
                    } else {
                        total_recv_buf.reserve(recv_len);
                    }

                    for (int i = 0; i < recv_len; i++) {
                        total_recv_buf.push_back(recv_buf[i]);
                    }

                    memset(recv_buf, 0, RECVBUF * sizeof(uint8_t));
                    delete[] recv_buf;
                    recv_buf = nullptr;
                }
                
                is_RecvRun = true;
            }
        }
    });

    serial_thread = std::thread([&]() {
        LOG_DEBUG("[TaskSerial]http thread is run\n");

        while (true) {
            while (taskinfo.taskstate != taskState::start) {
                std::unique_lock<std::mutex> lock(handle_mutex);

                LOG_DEBUG("[TaskSerial]work wait\n");
                if (!isStopThread) {
                    handle_cv.wait(lock);
                } 
                if (isStopThread) {
                    return;
                }
                LOG_DEBUG("[TaskSerial]work start\n");

                taskinfo.taskstate = taskState::start;
            }

            if (is_loadDevList == false && is_loadSerialList == false && is_mergeDevAndSerial == true) {
                loadAllList();
            } else {
                serialTaskRun();
            }
        }
    });
}

TaskSerial::~TaskSerial() {
    {
        std::unique_lock<std::mutex> lock(handle_mutex);
        isStopThread = true;
        taskinfo.taskstate = taskState::cancel;
        handle_cv.notify_all(); // 通知所有等待的线程
    }
    serial_thread.join();
}

void TaskSerial::start() {
    LOG_DEBUG("[TaskSerial]work start, previous state: %d\n", taskinfo.taskstate);

    std::unique_lock<std::mutex> lock(handle_mutex);

    taskinfo.taskstate = taskState::start;
    handle_cv.notify_all();
}

void TaskSerial::pause() {
    LOG_DEBUG("[TaskSerial]work pause, previous state: %d\n", taskinfo.taskstate);
    
    std::unique_lock<std::mutex> lock(handle_mutex);

    taskinfo.taskstate = taskState::stop;
}

void TaskSerial::resume() {
    LOG_DEBUG("[TaskSerial]work resume, previous state: %d\n", taskinfo.taskstate);
    
    std::unique_lock<std::mutex> lock(handle_mutex);

    if (taskinfo.taskstate != taskState::cancel) {
        taskinfo.taskstate = taskState::start;
    }
    handle_cv.notify_all();
}

void TaskSerial::stop() {
    LOG_DEBUG("[TaskSerial]work stop, previous state: %d\n", taskinfo.taskstate);

    std::unique_lock<std::mutex> lock(handle_mutex);

    taskinfo.taskstate = taskState::cancel;
}

void TaskSerial::loadAllList() {
    for (auto& serial : serialList) {
        // 判断是不是485串口
        if (serial.SerialName == "RS485-1" || 
            serial.SerialName == "RS485-2" ||
            serial.SerialName == "RS485-3" ||
            serial.SerialName == "RS485-4" ||
            serial.SerialName == "RS485-5" ||
            serial.SerialName == "RS485-6" ||
            serial.SerialName == "RS485-7" ||
            serial.SerialName == "RS485-8" ) 
        {
            // 创建一个485串口
            serialInfoMap[serial.name] = std::make_shared<Serial_485>(serial);
            LOG_DEBUG("create serial_485 success {}/{}/{}/{}/{}/{}/{}/{}/{}\n", 
                        serial.name, serial.SerialName, serial.SerialId, 
                        serial.serialParamInfo.baudrate, 
                        serial.serialParamInfo.databit,
                        serial.serialParamInfo.parity,
                        serial.serialParamInfo.stopbit,
                        serial.serialParamInfo.vmin,
                        serial.serialParamInfo.vtime);
        }
    }
    for (auto dev : devList) {
        for (auto& serial : serialInfoMap) {
            if (dev.serialInfo.name == serial.first) {
                if ("modbus" == dev.protocol) {
                    protocolInfoMap[dev.devName] = std::make_shared<Modbus>(serial.second.get());

                    break;
                }
            }
        }
    }

    is_mergeDevAndSerial = false;
}

void TaskSerial::serialTaskRun() {
    std::unique_lock<std::mutex> list_lck(list_mutex);
    for (auto it : protocolInfoMap) {
        std::vector<s_RTSrcInfo> v_rtSrcInfo;
        bool send_res = true;

        pointCount = 0;
        pit = it.second;

        while (true) {
            s_RTSrcInfo rtSrcInfo;

            send_res = sendPacketProc(rtSrcInfo);

            if (send_res) {
                v_rtSrcInfo.push_back(rtSrcInfo);
            } else {
                break;
                LOG_DEBUG("send protocol end\n");
            }
        }

        handleAllRecvData(v_rtSrcInfo);

        std::pair<std::string, std::vector<s_RTSrcInfo>> map_singleRTSrcInfo;
        map_singleRTSrcInfo = std::make_pair(it.first, v_singleRTSrcInfo);

        LOG_DEBUG("publish rtSrcInfo\n");
        MessageQueue< std::pair<std::string, std::vector<s_RTSrcInfo>> >::getInstance().publish("TaskSerial/seriesRTSrcInfo", map_singleRTSrcInfo);
    }
}

bool TaskSerial::sendPacketProc(s_RTSrcInfo& rtSrcInfo) {
    uint8_t send_buf[SENDBUF];
    int send_len = -1;
    bool send_res = false;

    std::weak_ptr<Protocol> send_pit = pit;
    if (auto sharedPtr = send_pit.lock()) {
        send_len = sharedPtr->AssemblePacket(send_buf);

        if (-1 != send_len) {
            sharedPtr->send((char*)send_buf);

            rtSrcInfo.sourceName = v_point[pointCount].name;
            pointCount++;

            is_RecvRun = false;
            recv_cv.notify_all();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            bool is_recvAll = false; // 判断是否收到的完整数据
            while (!is_recvAll) {
                is_recvAll = parsePacketProc(rtSrcInfo);
            }

            send_res = true;
        } else {
            send_res = false;
        }

    } else {
        send_res = false;
        LOG_ERROR("The object has been destroyed.\n");
    }

    return send_res;
}

bool TaskSerial::parsePacketProc(s_RTSrcInfo& rtSrcInfo) {
    std::unique_lock<std::mutex> lock(recv_mutex);

    bool is_recvAll = false;
    int len = total_recv_buf.size();

    if (len) {
        uint8_t recv_buf[len];

        for (int i = 0; i < len; i++) {
            recv_buf[i] = total_recv_buf[i];
        }

        int parse_len = 0;
        std::weak_ptr<Protocol> parse_pit = pit;
        if (auto sharedPtr = parse_pit.lock()) {
            parse_len = sharedPtr->ParsePacket(recv_buf, len);

            if (parse_len == 0) {
                LOG_DEBUG("crc check success, parse failed\n");
                rtSrcInfo.value.idata = 0; // 这里的value用来存长度，buf存原始报文，传出去还要再解析一下
                is_recvAll = true;

                total_recv_buf.clear();
            } else if (parse_len == -1) { // 如果收到的报文没有通过crc校验，那么延时5s等待收全
                LOG_DEBUG("crc check failed\n");
                rtSrcInfo.value.idata = -1;
                // checkCount++;
                is_recvAll = false;
                is_RecvRun = false;
                recv_cv.notify_all();
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            } else {
                LOG_DEBUG("crc check success, parse success\n");
                LOG_DEBUG("parse_len:{}\n", parse_len);

                rtSrcInfo.value.idata = parse_len;
                for (int i = 0; i < parse_len; i++) {
                    rtSrcInfo.buf.push_back(recv_buf[i]);
                }
                is_recvAll = true;

                total_recv_buf.clear();
            }
        } else {
            LOG_ERROR("The object has been destroyed.\n");
        }
    } else {
        is_RecvRun = false;
        recv_cv.notify_all();
    }

    return is_recvAll;
}

void TaskSerial::handleAllRecvData(vector<s_RTSrcInfo>& v_rtSrcInfo) {
    v_singleRTSrcInfo.clear();
    for (auto it : v_rtSrcInfo) {
        if (it.sourceName == "设备信息") {
            handleRecvData(it, 3300, 3321);
        } else if (it.sourceName == "运行信息") {
            handleRecvData(it, 3600, 3614);
            handleRecvData(it, 3618, 3667);
        } else if (it.sourceName == "电池组信息") {
            handleRecvData(it, 3680, 3694);
        } else if (it.sourceName == "运行参数") {
            handleRecvData(it, 3900, 3904);
            handleRecvData(it, 3906, 3923);
            handleRecvData(it, 3929, 3955);
        } else if (it.sourceName == "保护参数") {
            handleRecvData(it, 3800, 3845);
        } else if (it.sourceName == "采样校正") {
            handleRecvData(it, 5000, 5017);
            handleRecvData(it, 5024, 5037);
        } else if (it.sourceName == "并离网模式设置") {
            handleRecvData(it, 8000, 8012);
            handleRecvData(it, 8014, 8019);
        } else if (it.sourceName == "无功调节") {
            handleRecvData(it, 8030, 8040);
        } else if (it.sourceName == "低电压穿越参数") {
            handleRecvData(it, 8100, 8110);
        } else if (it.sourceName == "高电压穿越参数") {
            handleRecvData(it, 8150, 8159);
        } else if (it.sourceName == "功率缓启") {
            handleRecvData(it, 8200, 8204);
        } else {
            LOG_ERROR("recv buf no name\n");
        }
    }
}

void TaskSerial::handleRecvData(s_RTSrcInfo& it, const int&& haddr, const int&& taddr) {
    int count = 0;
    int match = 0;
    if (it.value.idata != 0 && it.value.idata != -1) {
        for (auto pit : v_points) {
            int merged = (pit.mpkg[0].reg[0] << 8) | pit.mpkg[0].reg[1];
            LOG_DEBUG("merged:{}\n", merged);

            s_RTSrcInfo RTSrcInfo;
            RTSrcInfo.sourceName = pit.name;
            RTSrcInfo.unit = pit.unit;

            if (merged >= haddr && merged <= taddr) {
                int value = 0;
                if (pit.dataType == "U16" || pit.dataType == "S16") {
                    uint8_t tmp[2];
                    tmp[0] = it.buf[count++];
                    tmp[1] = it.buf[count++];
                    value = (tmp[0] << 8) | tmp[1];
                } else if (pit.dataType == "U32" || pit.dataType == "S32") {
                    uint8_t tmp[4];
                    for (int i = 0; i < 4; i++) {
                        tmp[i] = it.buf[count++];
                    }
                    value = (tmp[0] << 32) | (tmp[1] << 16) | (tmp[2] << 8) | tmp[3];
                } else if (pit.dataType == "U8") {
                    uint8_t tmp;
                    tmp = it.buf[count++];
                    value = tmp;
                } else if (pit.dataType == "ASCII") {
                    uint8_t tmp[20];
                    for (int i = 0; i < 20; i++) {
                        tmp[i] = it.buf[count++];
                    }
                }

                if (pit.factor != 1.0) {
                    double dvalue = value * pit.factor;
                    RTSrcInfo.value.ddata = dvalue;
                    RTSrcInfo.value.idata = -1;
                    RTSrcInfo.value.sData = "NULL";
                } else {
                    RTSrcInfo.value.idata = value;
                    RTSrcInfo.value.ddata = -1.0;
                    RTSrcInfo.value.sData = "NULL";
                }

                match = 1;
                v_singleRTSrcInfo.push_back(RTSrcInfo);
            } else {
                LOG_ERROR("no match reg addr\n");
            }
        }
    }
}

int TaskSerial::loadPcsProtocolByJson(const std::string filename) {
    // 打开JSON文件
    std::ifstream file(filename);
    if (!file.is_open()) {
        // 文件打开失败，进行相应的处理
        LOG_ERROR("文件打开失败\n");
        return 0;
    } else {
        // 文件成功打开，继续读取文件内容
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        LOG_DEBUG("Json解析: {}\n", content.c_str());

        // 解析JSON数据
        json jsonData;
        try {
            jsonData = json::parse(content);

            // 验证JSON数据的有效性
            if (!jsonData.is_object()) {
                throw std::runtime_error("Invalid JSON data: not an object");
            }

            // 获取字段的值
            for (auto item : jsonData["points"]) {
                modbusPoint point;
                point.name = item["name"].get<std::string>();
                point.dataType = item["dataType"].get<std::string>();
                point.factor = item["factor"];
                point.unit = item["unit"].get<std::string>();

                modbusPkg pkg;
                std::string hexString = std::to_string(item["addr"].get<int>());
                int hexValue = std::stoi(hexString, nullptr, 10);
                pkg.reg = new uint8_t[2];
                pkg.reg[0] = (hexValue >> 8) & 0xFF;
                pkg.reg[1] = hexValue & 0xFF;
                point.mpkg.push_back(pkg);

                LOG_DEBUG("point.name: {} / point.dataType: {} / point.factor: {} / point.unit: {} / hexString: {}\n", point.name, point.dataType, point.factor, point.unit, hexString);
                v_points.push_back(point);
            }
        } catch(const std::exception& e) {
            // Json解析错误
            LOG_ERROR("Json解析错误\n");
            return 0;
        }

        file.close();
    }

    return 1;
}
