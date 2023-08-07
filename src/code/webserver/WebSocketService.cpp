#include "WebSocketService.h"

using namespace NetWorkMiddleware;

WebSocketService::WebSocketService() {
    // 创建条件变量和互斥量
    std::condition_variable cv;
    std::mutex cv_m;
    bool ready = false;

    // 在连接建立时将连接句柄添加到集合中
    echo_server.set_open_handler([&](connection_hdl hdl) {
        connections.insert(hdl);
    });

    // 在连接关闭时从集合中移除连接句柄
    echo_server.set_close_handler([&](connection_hdl hdl) {
        connections.erase(hdl);
    });

    echo_server.init_asio();
    echo_server.set_reuse_addr(true); // 添加这一行，启用地址重用
    echo_server.listen(9002);
    echo_server.start_accept();

    echo_server.set_message_handler(&on_message);

    // 在构造函数中创建线程
    server_thread = std::thread([&]() {
        {
            std::lock_guard<std::mutex> lk(cv_m);
            ready = true;
        }
        cv.notify_one();

        echo_server.run();
    });

    {
        std::unique_lock<std::mutex> lk(cv_m);
        cv.wait(lk, [&] { return ready; });
    }
}

WebSocketService::~WebSocketService() {
    echo_server.stop();
}

void WebSocketService::receive(std::string& data) {
    data = message;
    std::cout << "收到H5页面的消息: " << data << std::endl;
}

void WebSocketService::send(std::string& data) {
    // 发送数据给所有连接的客户端
    for (auto& con : connections) {
        echo_server.send(con, data, websocketpp::frame::opcode::text);
    }
}

void WebSocketService::on_message(connection_hdl hdl, server<asio>::message_ptr msg) {
    std::string message = msg->get_payload();
}
