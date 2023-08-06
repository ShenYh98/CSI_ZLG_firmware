#pragma once

#include "NetworkService.h"

#include "httplib/httplib.hpp"

namespace NetWorkMiddleware {

class HttpService : public NetworkService {
public:
    HttpService(std::string httpPath);
    ~HttpService();

    void receive(std::string& data) override;

    void send(std::string& data) override;

private:
    void handle_request(const httplib::Request& req, httplib::Response& res);

private:
    httplib::Server server;
    std::string recv_data;
    std::string response_data;
    std::thread server_thread;
};

}
