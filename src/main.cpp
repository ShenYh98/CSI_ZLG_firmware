#include <iostream>
#include <fstream>
#include <cstring>

#include "nlohmann/json.hpp"

#include "code/webserver/WebSocketService.h"

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

int main() {
    NetworkService* websocketService = new WebSocketService();

    while (true) {
        double voltage = 220.0;
        double current = 10.0;
        double temperature = 25.0;

        std::string data = std::to_string(voltage) + "," + std::to_string(current) + "," + std::to_string(temperature);
        websocketService->send(data);
        std::cout << data << std::endl;
        std::this_thread::sleep_for(3s);
    }
    
    delete(websocketService);

    return 0;
}