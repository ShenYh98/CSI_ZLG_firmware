#pragma once

#include "NetworkService.h"

#include "httplib/httplib.hpp"

namespace NetWorkMiddleware {

class HttpService : public NetworkService {
public:
    HttpService(const std::string& httpPath, int port);
    ~HttpService();

    void receive(std::string& data) override;

    void send(std::string& data) override;

private:
    void handle_request(const httplib::Request& req, httplib::Response& res);
    int getResponseTime(const std::string recv_data);
    int getCodeError(const std::string response_data);

private:
    httplib::Server server;
    std::string response_data;
    std::thread server_thread;

    std::queue<std::string> recv_queue;     // 接收http请求的队列，用于处理并发请求
    std::mutex queue_mutex;                 // 队列锁，保证recv_queue不会被同时pop和push
    int lastQueueSize;                      // 记录上一次队列大小

    std::condition_variable handle_cv;
    std::mutex handle_cv_m;
};

}