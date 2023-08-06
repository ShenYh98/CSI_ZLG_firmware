#include "HttpService.h"

using namespace NetWorkMiddleware;

HttpService::HttpService(std::string httpPath) {
    // 默认回复ok
    response_data = "ok";

    // 创建条件变量和互斥量
    std::condition_variable cv;
    std::mutex cv_m;
    bool ready = false;

    // 处理路径为 /api 的 POST 请求
    server.Post("/api", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        handle_request(req, res);
    });

    // 在构造函数中创建线程,线程里必须完成监听，主函数才能执行
    server_thread = std::thread([&]() {
        {
            std::lock_guard<std::mutex> lk(cv_m);
            ready = true;
        }
        cv.notify_one();
        
        server.listen(httpPath, 9003);
    });

    {
        std::unique_lock<std::mutex> lk(cv_m);
        cv.wait(lk, [&] { return ready; });
    }
}

HttpService::~HttpService() {
    server.stop();
}

void HttpService::receive(std::string& data) {
    data = recv_data;
    // recv_data.clear(); // 收到消息后清掉，准备接收新消息
}

void HttpService::send(std::string& data) {
    response_data = data;
}

void HttpService::handle_request(const httplib::Request& req, httplib::Response& res) {
    recv_data = req.body;

    // 处理接收到的消息，并返回响应给H5
    res.set_content(response_data, "text/plain");
}
