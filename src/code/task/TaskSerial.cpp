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
                                    protocolInfoMap[dev.devName] = std::make_unique<Modbus>(s_it->second.get());
                                }
                            }
                            // 这里最好加一个未匹配到串口名的错误检测
                        }

                        break;
                    }
                }  
            } else if (devList.size() > msg.size()) { // 删除
                for ( auto dev = devList.begin(); dev != devList.end(); ) {
                    auto it = std::find_if(msg.begin(), msg.end(), [&](const DevInfo& v) {
                        return v.devName == dev->devName;  // 自定义比较逻辑
                    });

                    if (it == msg.end()) {
                        devList.erase(dev);
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

                    if (match) {
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
                            serialInfoMap[serial.name] = std::make_unique<Serial_485>(serial);
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
                    auto it = std::find_if(msg.begin(), msg.end(), [&](const SerialIdInfo& s) {
                        return s.name == serial->name;  // 自定义比较逻辑
                    });

                    if (it == msg.end()) {
                        serialList.erase(serial);
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

                    if (match) {
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

                LOG_DEBUG("[TaskHttp]work wait\n");
                if (!isStopThread) {
                    handle_cv.wait(lock);
                } 
                if (isStopThread) {
                    return;
                }
                LOG_DEBUG("[TaskHttp]work start\n");

                taskinfo.taskstate = taskState::start;
            }

            if (is_loadDevList == false && is_loadSerialList == false && is_mergeDevAndSerial == true) {
                loadAllList();
            } else {
                {
                    std::unique_lock<std::mutex> list_lck(list_mutex);
                    for (auto& it : protocolInfoMap) {
                        uint8_t buf[8];
                        it.second->AssemblePacket(buf);
                        it.second->send((char*)buf);

                        // 添加延时
                        std::this_thread::sleep_for(std::chrono::milliseconds(200));
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
            serialInfoMap[serial.name] = std::make_unique<Serial_485>(serial);
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
                    protocolInfoMap[dev.devName] = std::make_unique<Modbus>(serial.second.get());

                    break;
                }
            }
        }
    }

    is_mergeDevAndSerial = false;
}
