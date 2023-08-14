#include <iostream>
#include <fstream>
#include <cstring>

#include <mutex>
#include <functional>
#include <condition_variable>

#include "code/webserver/WebSocketService.h"
#include "code/webserver/NetWorkLayerImp.h"
#include "code/webserver/HttpService.h"
#include "code/common/CommonLog.h"
#include "code/uart/Uart_485.h"

#include "code/common/ThreadPool.h"

#include "code/common/MessageQueue.hpp"

using namespace CommonLib;
using namespace NetWorkMiddleware;
using namespace UartMiddleware;

int main() {
    COMMONLOG_INIT("config/csi_config.json");
    // =================================websocket======================================
    // NetworkService* websocketService = new WebSocketService();
    // NetWorkLayer* sendCommonInfo = new NetWorkLayerImp(websocketService);

    // while (true) {
    //     sendCommonInfo->operation(taskId::common);

    //     std::this_thread::sleep_for(3s);
    // }
    
    // delete(websocketService);
    // delete(sendCommonInfo);

    // ===================================http====================================
    std::string path = "192.168.1.136";
    int port = 9000;
    NetworkService* httpService = new HttpService(path, port);
    NetWorkLayer* httpWork = new NetWorkLayerImp(httpService);

    while (true) {
        httpWork->operation();
        std::this_thread::sleep_for(3s);
    }

    delete httpWork;
    delete httpService;
    //============================================uart==========================================
    // UartAbstract* uart_485_1 = new Uart_485("");
    // std::string str = "hello world";

    // while (1)
    // {
    //     uart_485_1->send(str.c_str());
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }

    //=============================================md=============================================
    // MessageQueue<int>::getInstance().subscribe("topic", [](const int& message) {
    //     // 处理订阅到的消息
    //     std::cout << "Received message: " << message << std::endl;
    // });

    // MessageQueue<int>::getInstance().publish("topic", 1);
    // MessageQueue<int>::getInstance().publish("topic", 2);
    // MessageQueue<int>::getInstance().publish("topic", 3);
    // MessageQueue<int>::getInstance().publish("topic", 4);
    // MessageQueue<int>::getInstance().publish("topic", 5);

    return 0;
}