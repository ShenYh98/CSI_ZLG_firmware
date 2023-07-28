#include <iostream>
#include <fstream>
#include <sstream>

#include "cjson/cJSON.h"
#include "cjson/cJSON_Utils.h"

using namespace std;

int main() {
    std::cout << "version 0.1" << endl;

    std::string path = "./config/csi_config.json";

    // 读取 JSON 文件
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "Failed to open JSON file." << std::endl;
        return 0;
    } else {
        // 将内容写到json_data里
        std::stringstream buffer;
        buffer <<file.rdbuf();
        std::string json_data = buffer.str();

        // 解析JSON字符串
        cJSON* root = cJSON_Parse(json_data.c_str());
        
        // 检查是否解析成功
        if (root == nullptr) {
            std::cout << "JSON解析错误: " << cJSON_GetErrorPtr() << std::endl;
            return 0;
        }
        
        // 获取JSON属性值
        cJSON* json_msg = cJSON_GetObjectItem(root, "msg");
        if (json_msg != nullptr && cJSON_IsString(json_msg)) {
            std::string msg = json_msg->valuestring;
            std::cout << "msg: " << msg << std::endl;
        }

        // 释放JSON资源
        cJSON_Delete(root);
        file.close();
    }

    return 0;
}
