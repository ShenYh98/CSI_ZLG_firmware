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

int main() {
    std::string filename = "/var/www/html/json/user.json";
    parseJSONFile(filename);

    return 0;
}