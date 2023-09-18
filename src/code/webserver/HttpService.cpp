#include "HttpService.h"

using namespace NetWorkMiddleware;

HttpService::HttpService(const std::string& httpPath, int port) {
    // 默认回复ok
    response_data = "ok";
    lastQueueSize = 0;

    // 创建条件变量和互斥量
    std::condition_variable http_cv;
    std::mutex http_cv_m;
    bool ready = false;

    // 处理路径为 /api 的 POST 请求
    server.Post("/api", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "*");
        res.set_header("Access-Control-Allow-Headers", "*");

        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            recv_queue.push(req.body);
        }
        printf("recv http request!\n");
        handle_request(req, res);
    });

    // 在构造函数中创建线程,线程里必须完成监听，主函数才能执行
    server_thread = std::thread([&]() {
        {
            std::lock_guard<std::mutex> lk(http_cv_m);
            ready = true;
        }
        http_cv.notify_one();
        
        server.listen(httpPath, port);
    });

    {
        std::unique_lock<std::mutex> lk(http_cv_m);
        http_cv.wait(lk, [&] { return ready; });
    }
}

HttpService::~HttpService() {
    server.stop();
}

void HttpService::receive(std::string& data) {
    if (recv_queue.empty()) {
        lastQueueSize = recv_queue.size();
    }
    int curQueueSize = recv_queue.size();
    if (!recv_queue.empty() && curQueueSize != lastQueueSize) {
        data = recv_queue.front();
        lastQueueSize = curQueueSize;
    } else {
        data = "";
    }
}

void HttpService::send(std::string& data) {
    response_data = data;
    handle_cv.notify_all();
}

void HttpService::handle_request(const httplib::Request& req, httplib::Response& res) {
    LOG_DEBUG("[handle_request]req.body: {}\n", req.body.c_str());
    std::string recv_data;
    
    // 处理接收到的消息，并返回响应给 H5
    if (req.method == "POST") {
        // 处理 H5 发来的请求
        recv_data = req.body;
    }

    while(true) {
        std::unique_lock<std::mutex> lck(handle_cv_m);
        handle_cv.wait(lck);
        if (recv_queue.front() == recv_data && !recv_queue.empty()) {
            break;
        }
    }

    LOG_DEBUG("[handle_request]======================http接受回复======================\n");
    LOG_DEBUG("[handle_request]response_data: {}\n", response_data.c_str());

    if (!response_data.empty()) {
        int codeError = getCodeError(response_data);

        // 返回响应给客户端
        if (codeError == 200) {
            res.status = codeError; // 设置响应状态码
            // 设置响应头部
            res.set_header("Content-Type", "application/json");
            // 设置响应体
            res.set_content(response_data, "application/json");
        } else {
            res.status = codeError; // 设置响应状态码
            // 设置响应体
            res.set_content(response_data, "text/plain");
        }
    }

    response_data.clear();

    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        recv_queue.pop();
    }
}

int HttpService::getCodeError(const std::string response_data) {
    // 解析JSON数据
    json http_jsonData;
    int http_codeError;
    try {
        http_jsonData = json::parse(response_data);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        // 获取字段的值
        http_codeError = http_jsonData["errorCode"].get<int>();
        LOG_ERROR("错误码: {}\n", http_codeError);

    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }

    return http_codeError;
}

int HttpService::getResponseTime(const std::string recv_data) {
    // 解析JSON数据
    json http_jsonData;
    int http_responseTime;
    try {
        http_jsonData = json::parse(recv_data);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        // 获取字段的值
        http_responseTime = http_jsonData["responseTime"].get<int>();
        LOG_DEBUG("响应时间:{}\n", http_responseTime);
        
    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }

    return http_responseTime;
}