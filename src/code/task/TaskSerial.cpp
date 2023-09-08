#include "TaskSerial.h"

using namespace TaskMiddleWare;

TaskSerial::TaskSerial() {
    is_loadDevList = true;
    is_loadSerialList = true;
    is_mergeDevAndSerial = true;
    is_RecvRun = true;
    isStopThread = false;

    devList.clear();
    serialList.clear();
    serialInfoMap.clear();
    protocolInfoMap.clear();

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
                    recv_len = sharedPtr->receive((char*)recv_buf);
                } else {
                    LOG_ERROR("The object has been destroyed.\n");
                }

                {
                    std::unique_lock<std::mutex> lock(recv_mutex);
                    LOG_DEBUG("recv_len:{}\n", recv_len);

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
                {
                    std::unique_lock<std::mutex> list_lck(list_mutex);
                    for (auto it : protocolInfoMap) {
                        std::vector<s_RTSrcInfo> v_rtSrcInfo;
                        int pointCount = 0;
                        pit = it.second;

                        while (true) {
                            // auto send_pit = pit; // 复制智能指针，避免段错误
                            uint8_t send_buf[SENDBUF];
                            s_RTSrcInfo rtSrcInfo;
                            int send_len = -1;

                            std::weak_ptr<Protocol> send_pit = pit;
                            if (auto sharedPtr = send_pit.lock()) {
                                send_len = sharedPtr->AssemblePacket(send_buf);

                                if (-1 != send_len) {
                                    sharedPtr->send((char*)send_buf);

                                    rtSrcInfo.sourceName = v_point[pointCount].name;
                                    rtSrcInfo.unit = "V";
                                    pointCount++;

                                    is_RecvRun = false;
                                    recv_cv.notify_all();
                                    std::this_thread::sleep_for(std::chrono::milliseconds(200));

                                    bool is_recvAll = false; // 判断是否收到的完整数据
                                    // int checkCount = 0;
                                    // while (!is_recvAll && checkCount < 2) {
                                    while (!is_recvAll) {
                                        {
                                            std::unique_lock<std::mutex> lock(recv_mutex);

                                            int len = total_recv_buf.size();

                                            if (len) {
                                                uint8_t recv_buf[len];

                                                for (int i = 0; i < len; i++) {
                                                    recv_buf[i] = total_recv_buf[i];
                                                }

                                                int parse_len = 0;
                                                std::weak_ptr<Protocol> parse_pit = pit;
                                                if (auto sharedPtr = parse_pit.lock()) {
                                                    // parse_len = sharedPtr->ParsePacket(recv_buf, len);

                                                    parse_len = sharedPtr->ParsePacket(recv_buf, len);

                                                    if (parse_len == 0) {
                                                        LOG_DEBUG("crc check success, parse failed\n");
                                                        rtSrcInfo.value = 0;
                                                        is_recvAll = true;

                                                        total_recv_buf.clear();
                                                    } else if (parse_len == -1) { // 如果收到的报文没有通过crc校验，那么延时5s等待收全
                                                        LOG_DEBUG("crc check failed\n");
                                                        rtSrcInfo.value = 0;
                                                        // checkCount++;
                                                        is_recvAll = false;
                                                        is_RecvRun = false;
                                                        recv_cv.notify_all();
                                                        std::this_thread::sleep_for(std::chrono::milliseconds(200));
                                                    } else {
                                                        LOG_DEBUG("crc check success, parse success\n");
                                                        // // 收到了报文回复
                                                        auto value = HexToDec(recv_buf, parse_len);

                                                        LOG_DEBUG("value:{} len:{}\n", value, parse_len);

                                                        rtSrcInfo.value = value;
                                                        is_recvAll = true;

                                                        total_recv_buf.clear();

                                                        // break;
                                                    }
                                                } else {
                                                    LOG_ERROR("The object has been destroyed.\n");
                                                }
                                            } else {
                                                is_RecvRun = false;
                                                recv_cv.notify_all();
                                            }
                                        }
                                    }

                                    v_rtSrcInfo.push_back(rtSrcInfo);
                                } else {
                                    break;
                                }

                            } else {
                                LOG_ERROR("The object has been destroyed.\n");
                            }
                        }

                        LOG_DEBUG("publish rtSrcInfo\n");
                        MessageQueue<std::vector<s_RTSrcInfo>>::getInstance().publish("TaskSerial/rtSrcInfo", v_rtSrcInfo);
                    }
                }
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

int TaskSerial::HexToDec(const uint8_t* data, std::size_t length) {
    std::ostringstream hexStream;
    for (std::size_t i = 0; i < length; ++i) {
        hexStream << std::hex  << std::setfill('0') << std::setw(2) << static_cast<int>(data[i]);
    }

    std::string hexString = hexStream.str();

    try {
        int result = std::stoi(hexString, nullptr, 16);
        return result;
    } catch (const std::invalid_argument& e) {
        return -1;
    }
}
