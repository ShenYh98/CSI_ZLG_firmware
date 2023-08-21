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

#include "code/common/CommonLog.h"
#include "code/common/ThreadPool.h"
#include "code/common/MessageQueue.hpp"

#include "code/setting/Setting.h"

#include "DataStructure.h"

using namespace CommonLib;
using namespace NetWorkMiddleware;
using namespace SerialMiddleware;
using namespace SettingMiddleware;

#define  TASK_MAX     12
#define  TASK_MIN     5

void TaskHttp () {
    std::string path = "192.168.8.113";
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
}

int main() {
    COMMONLOG_INIT("config/csi_config.json");

    ThreadPool taskThreadPool(TASK_MIN, TASK_MAX);
    taskThreadPool.Add(TaskHttp);
    taskThreadPool.Add(TaskWebSocket);
    taskThreadPool.Add(TaskSetting);

    //============================================Serial==========================================
    // SerialAbstract* Serial_485_1 = new Serial_485("");
    // std::string str = "hello world";

    // while (1)
    // {
    //     Serial_485_1->send(str.c_str());
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }

    //=============================================mq=============================================
    // MessageQueue<int>::getInstance().subscribe("topic", [](const int& message) {
    //     // 处理订阅到的消息
    //     std::cout << "Received message: " << message << std::endl;
    // });

    // MessageQueue<int>::getInstance().publish("topic", 1);
    // MessageQueue<int>::getInstance().publish("topic", 2);
    // MessageQueue<int>::getInstance().publish("topic", 3);
    // MessageQueue<int>::getInstance().publish("topic", 4);
    // MessageQueue<int>::getInstance().publish("topic", 5);

    //=============================================sq=============================================
    // using Message = std::string;  // 定义消息类型为std::string
    // using Response = std::string; // 定义响应类型为std::string

    // ServiceQueue<Message, Response>& queue = ServiceQueue<Message, Response>::getInstance();

    // // 订阅一个名为"hello"的主题，并为其定义一个回调函数
    // queue.service_subscribe("test", [](const Message& msg, std::function<void(const Response&)> responder) {
    //     std::cout << "Received: " << msg << std::endl;
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    //     responder("ok");
    // });

    

    while (true) {

    }

    return 0;
}