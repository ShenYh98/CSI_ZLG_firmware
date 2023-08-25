#include <iostream>
#include <fstream>
#include <cstring>

#include <mutex>
#include <functional>
#include <condition_variable>

#include "code/webserver/WebSocketService.h"
#include "code/webserver/NetWorkLayerImp.h"
#include "code/webserver/HttpService.h"

#include "code/serial/Serial_485.h"
#include "code/protocol/Modbus.h"

#include "code/common/CommonLog.h"
#include "code/common/ThreadPool.h"
#include "code/common/MessageQueue.hpp"

#include "code/setting/Setting.h"

#include "DataStructure.h"

using namespace CommonLib;
using namespace NetWorkMiddleware;
using namespace SerialMiddleware;
using namespace SettingMiddleware;
using namespace ProtocolMiddleware;

#define  TASK_MAX     12
#define  TASK_MIN     5

void TaskHttp () {
    std::string path = "192.168.1.136";
    int port = 9000;
    NetworkService* httpService = new HttpService(path, port);
    NetWorkLayer* httpWork = new NetWorkLayerImp(httpService);

    while (true) {
        httpWork->operation();
        std::this_thread::sleep_for(2s);
    }

    delete httpWork;
    delete httpService;
}

void TaskWebSocket() {
    NetworkService* websocketService = new WebSocketService();
    NetWorkLayer* websocketWork = new NetWorkLayerImp(websocketService);

    s_RTtask rttask;

    // 订阅实时监控任务
    MessageQueue<s_RTtask>::getInstance().subscribe("websocket/task", [&](const s_RTtask& task) {
        std::cout << "Received id: " << task.id << std::endl;
        std::cout << "Received dev_sn: " << task.dev_sn << std::endl;
        std::cout << "Received isOn: " << task.isOn << std::endl;

        rttask = task;
    });

    while (true) {
        if (rttask.isOn) { // 需要http下发启动指令，再去遥测
            websocketWork->operation(rttask.id);
        }
        
        std::this_thread::sleep_for(3s);
    }
    
    delete(websocketService);
    delete(websocketWork);
}

void TaskSetting() {
    Setting* setting = new Setting();

    while(true) {
    }

    delete(setting);
}

void TaskSerial() {
    std::vector<DevInfo> devList;
    std::vector<SerialIdInfo> serialList;

    std::unordered_map<std::string, SerialAbstract*> serialInfoMap;
    std::unordered_map<std::string, Protocol*> protocolInfoMap;

    bool is_loadDevList = true;
    bool is_loadSerialList = true;
    bool is_mergeDevAndSerial = true;

    MessageQueue<std::vector<DevInfo>>::getInstance().subscribe("Setting/DevList", [&](const std::vector<DevInfo>& msg) {
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
                                    protocolInfoMap[dev.devName] = new Modbus(s_it->second);
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
                    auto it = std::find_if(msg.begin(), msg.end(), [&](const DevInfo& v) {
                        return v.devName == protocol->first;  // 自定义比较逻辑
                    });

                    if (it == msg.end()) {
                        delete(protocol->second);
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
                            serialInfoMap[serial.name] = new Serial_485(serial);
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
                    auto it = std::find_if(msg.begin(), msg.end(), [&](const SerialIdInfo& s) {
                        return s.name == serial->first;  // 自定义比较逻辑
                    });

                    if (it == msg.end()) {
                        delete(serial->second);
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
    
    while(true) {
        if (is_loadDevList == false && is_loadSerialList == false && is_mergeDevAndSerial == true) {
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
                    serialInfoMap[serial.name] = new Serial_485(serial);
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
                for (auto serial : serialInfoMap) {
                    if (dev.serialInfo.name == serial.first) {
                        if ("modbus" == dev.protocol) {
                            protocolInfoMap[dev.devName] = new Modbus(serial.second);

                            break;
                        }
                    }
                }
            }

            is_mergeDevAndSerial = false;
        } else {
            for (auto it : protocolInfoMap) {
                uint8_t buf[8];
                it.second->AssemblePacket(buf);
                it.second->send((char*)buf);

                // 添加延时
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        }
    }
}

int main() {
    COMMONLOG_INIT("config/csi_config.json");

    ThreadPool taskThreadPool(TASK_MIN, TASK_MAX);
    taskThreadPool.Add(TaskSerial);
    taskThreadPool.Add(TaskHttp);
    taskThreadPool.Add(TaskWebSocket);
    taskThreadPool.Add(TaskSetting);

    while (true) {
    }

    //============================================Serial==========================================
    // SerialAbstract* Serial_485_1 = new Serial_485("");
    // std::string str = "hello world";

    // while (1)
    // {
    //     Serial_485_1->send(str.c_str());
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }

    return 0;
}