#include "HttpService.h"

using namespace NetWorkMiddleware;

HttpService::HttpService(const std::string& httpPath, int port) {
    // 默认回复ok
    response_data = "ok";

    sem_init(&sem, 0, 0);

    // 创建条件变量和互斥量
    std::condition_variable cv;
    std::mutex cv_m;
    bool ready = false;

    // 处理路径为 /api 的 POST 请求
    server.Post("/api", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "*");
        res.set_header("Access-Control-Allow-Headers", "*");
        handle_request(req, res);
    });

    // 在构造函数中创建线程,线程里必须完成监听，主函数才能执行
    server_thread = std::thread([&]() {
        {
            std::lock_guard<std::mutex> lk(cv_m);
            ready = true;
        }
        cv.notify_one();
        
        server.listen(httpPath, port);
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
    printf("[receive]data:%s /recv_data:%s\n", data.c_str(), recv_data.c_str());
    data = recv_data;
    // recv_data.clear(); // 收到消息后清掉，准备接收新消息
}

void HttpService::send(std::string& data) {
    response_data = data;
}

void HttpService::handle_request(const httplib::Request& req, httplib::Response& res) {
    printf("[handle_request]req.body:%s /recv_data:%s\n", req.body.c_str(), recv_data.c_str());
    // 处理接收到的消息，并返回响应给 H5
    if (req.method == "POST") {
        // 处理 H5 发来的请求
        recv_data = req.body;
    }

    // 等待网络层完成操作
    sem_wait(&sem);

    // 根据网络层的操作结果设置响应数据
    res.set_content(response_data, "text/plain");

    recv_data.clear();
    
    // 返回响应给客户端
    // if (name == http_name && password == http_password) {
    //     printf("用户密码正确！\n");
    //     res.status = 200; // 设置响应状态码
    //     // 设置响应头部
    //     res.set_header("Content-Type", "application/json");
    //     // 设置响应体
    //     res.set_content("用户密码正确", "application/json");
    // } else {
    //     res.status = 400; // 设置响应状态码
    //     // 设置响应体
    //     res.set_content("请求错误", "text/plain");
    // }
}
