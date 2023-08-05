#pragma once

#include "NetworkService.h"

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

using websocketpp::server;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::connection_hdl;
using websocketpp::config::asio;
using namespace std::placeholders;
using namespace std::chrono_literals;

class WebSocketService : public NetworkService {
public:
    WebSocketService();
    ~WebSocketService();

    void receive(std::string& data) override;

    void send(std::string& data) override;

private:
    std::set<connection_hdl, std::owner_less<connection_hdl>> connections;
    server<asio> echo_server;
    std::string message;
    std::thread server_thread;

private:
    static void on_message(connection_hdl hdl, server<asio>::message_ptr msg);
};
