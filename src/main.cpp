#include <iostream>
#include <fstream>
#include <cstring>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

void parseJSONFile(const std::string& filename) {
    // 打开JSON文件
    std::ifstream file(filename);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // 解析JSON数据
    json jsonData = json::parse(content);

    // 获取字段的值
    std::string name = jsonData["name"];
    int password = jsonData["password"];
}


#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

using websocketpp::server;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

using websocketpp::connection_hdl;
using websocketpp::config::asio;

void on_message(connection_hdl hdl, server<asio>::message_ptr msg) {
    std::string message = msg->get_payload();
    std::cout << "收到客户端消息：" << message << std::endl;
    
    // 进行消息处理或响应
    // ...
}

int main() {
    using namespace std::placeholders;
    
    server<asio> echo_server;
    
    try {
        echo_server.set_message_handler(std::bind(&on_message, _1, _2));

        echo_server.init_asio();
        echo_server.listen(9002);

        echo_server.start_accept();
        echo_server.run();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}