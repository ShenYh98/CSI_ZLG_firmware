#include <iostream>
#include <fstream>
#include <cstring>

#include "code/webserver/WebSocketService.h"
#include "code/webserver/NetWorkLayerImp.h"
#include "code/webserver/HttpService.h"
#include "code/common/CommonLog.h"
#include "code/uart/Uart_485.h"

using namespace CommonLib;
using namespace NetWorkMiddleware;
using namespace UartMiddleware;



struct Message {
    std::string id;
    std::string content;
};

struct Subscriber {
    std::string id;
    std::function<void(const Message&)> callback;
};

class MessageQueue {
public:
    void publish(const Message& message);
    void subscribe(const std::string& topic, std::function<void(const Message&)> callback);

private:
    std::mutex mtx;
    std::map<std::string, std::vector<Subscriber>> subscribers;
};

void MessageQueue::publish(const Message& message) {
    std::unique_lock<std::mutex> lock(mtx);
    const std::string& topic = message.id;

    if (subscribers.find(topic) != subscribers.end()) {
        for (const Subscriber& subscriber : subscribers[topic]) {
            subscriber.callback(message);
        }
    }
}

void MessageQueue::subscribe(const std::string& topic, std::function<void(const Message&)> callback) {
    std::thread([this, topic, callback]() {
        Subscriber subscriber;
        subscriber.id = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
        subscriber.callback = callback;

        std::unique_lock<std::mutex> lock(mtx);
        subscribers[topic].push_back(subscriber);
        lock.unlock();

        while (true) {
            // TODO: 在这里等待消息的到来
        }
    }).detach();
}



int main() {
    // COMMONLOG_INIT("config/csi_config.json");

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
    // std::string path = "192.168.1.136";
    // int port = 9000;
    // NetworkService* httpService = new HttpService(path, port);

    // NetWorkLayer* getPassWord = new NetWorkLayerImp(httpService);

    // while (true) {
    //     getPassWord->operation();
    //     std::this_thread::sleep_for(3s);
    // }

    // delete getPassWord;
    // delete httpService;
    
    //============================================uart==========================================
    // UartAbstract* uart_485_1 = new Uart_485("");
    // std::string str = "hello world";

    // while (1)
    // {
    //     uart_485_1->send(str.c_str());
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }

    //=============================================md=============================================
    MessageQueue messageQueue;

    // 订阅者1的处理函数
    std::function<void(const Message&)> subscriber1Handler = [](const Message& message) {
        std::cout << "Subscriber 1: Received message with id " << message.id << ": " << message.content << std::endl;
    };

    // 订阅者2的处理函数
    std::function<void(const Message&)> subscriber2Handler = [](const Message& message) {
        std::cout << "Subscriber 2: Received message with id " << message.id << ": " << message.content << std::endl;
    };

    // 订阅者3的处理函数
    std::function<void(const Message&)> subscriber3Handler = [](const Message& message) {
        std::cout << "Subscriber 3: Received message with id " << message.id << ": " << message.content << std::endl;
    };

    // 订阅者4的处理函数
    std::function<void(const Message&)> subscriber4Handler = [&](const Message& message) {
        std::cout << "Subscriber 4: Received message with id " << message.id << ": " << message.content << std::endl;
    };

    // 订阅话题A的消息
    messageQueue.subscribe("A", subscriber1Handler);
    messageQueue.subscribe("A", subscriber2Handler);

    // 订阅话题B的消息
    messageQueue.subscribe("B", subscriber3Handler);
    messageQueue.subscribe("B", subscriber4Handler);

    // 发布一个消息到话题A
    while (1) {
        Message message;
        message.id = "A";
        message.content = "Hello, Topic A!";
        messageQueue.publish(message);

        // 等待1秒，模拟消息处理时间
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    return 0;
}