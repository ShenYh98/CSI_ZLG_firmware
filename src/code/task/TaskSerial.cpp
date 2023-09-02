#include "TaskSerial.h"

using namespace TaskMiddleWare;

TaskSerial::TaskSerial() {
    is_loadDevList = true;
    is_loadSerialList = true;
    is_mergeDevAndSerial = true;

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
                        while (true) {
                            auto pit = it.second;
                            uint8_t send_buf[SENDBUF];
                            auto send_len = pit->AssemblePacket(send_buf);
                            if (-1 != send_len) {
                                pit->send((char*)send_buf);
                                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                            } else {
                                break;
                            }

                            uint8_t recv_buf[RECVBUF];
                            int recv_len = 0;
                            recv_len = pit->receive((char*)recv_buf);

                            if (recv_len != 0) {
                                auto parse_len = pit->ParsePacket(recv_buf, recv_len);
                                if (parse_len) {
                                    // 收到了报文回复
                                    LOG_DEBUG("[TaskSerial]recv len:{}\n", recv_len);

                                    printf("0x%02x 0x%02x\n", recv_buf[0], recv_buf[1]);
                                }
                            }
                        }
                    }
                }
            }
        }
    });
}

TaskSerial::~TaskSerial() {

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
