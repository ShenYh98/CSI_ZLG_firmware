#include <iostream>
#include <fstream>
#include <cstring>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

#include "code/webserver/WebSocketService.h"
#include "code/webserver/NetWorkLayerImp.h"

using namespace NetWorkMiddleware;

#include "code/webserver/HttpService.h"

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

int main() {
    // NetworkService* websocketService = new WebSocketService();
    // NetWorkLayer* sendCommonInfo = new NetWorkLayerImp(websocketService);

    // while (true) {
    //     sendCommonInfo->operation(taskId::common);

    //     std::this_thread::sleep_for(3s);
    // }
    
    // delete(websocketService);
    // delete(sendCommonInfo);

    NetworkService* httpService = new HttpService("192.168.193.134");

    std::string send_data = "send ok";
    httpService->send(send_data);

    while (true) {
        std::string data;
        httpService->receive(data);
        if (!data.empty()) {
            std::cout << "从h5收到的消息：" << data << std::endl;
        }
    }

    return 0;
}