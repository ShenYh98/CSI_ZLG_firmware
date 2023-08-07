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
    std::string recv_data;
    std::string response_data;
    std::thread server_thread;
};

}
