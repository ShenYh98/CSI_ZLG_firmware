#pragma once

#include <iostream>

#include <mutex>
#include <functional>
#include <condition_variable>

#include "ThreadPool.h"
#include "nlohmann/json.hpp"

#define  THREAD_MAX     3
#define  THREAD_MIN     1

namespace CommonLib {

template<typename Message>
struct Subscriber {
    std::string topic;
    std::function<void(const Message&)> callback;
};

template<typename Message>
class MessageQueue {
public:
    MessageQueue() : threadPool(THREAD_MIN, THREAD_MAX) {}
    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;

    static MessageQueue<Message>& getInstance() {
        static MessageQueue<Message> instance;
        return instance;
    }

    // 订阅在初始化处完成
    void subscribe(const std::string& topic, const std::function<void(const Message&)>& callback) {
        std::lock_guard<std::mutex> lock(mutex);
        subscribers.push_back({ topic, callback });
    }

    void publish(const std::string& topic, const Message& message) {
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& subscriber : subscribers) {
            if (subscriber.topic == topic) {
                threadPool.Add(subscriber.callback, message);
            }
        }
    }
    
private:
    std::vector<Subscriber<Message>> subscribers;
    ThreadPool threadPool;
    std::mutex mutex;
};

}
