#include "NetWorkLayerImp.h"

using namespace NetWorkMiddleware;

NetWorkLayerImp::NetWorkLayerImp(NetworkService* networkSrv) {
    _networkSrv = networkSrv;
}

NetWorkLayerImp::~NetWorkLayerImp() {

}

void NetWorkLayerImp::receive(std::string& data) {
    _networkSrv->receive(data);
}

void  NetWorkLayerImp::send(std::string& data) {
    _networkSrv->send(data);
}

void  NetWorkLayerImp::operation() {
    int taskId = -1;
    std::string recv_data;
    this->receive(recv_data);

    if (!recv_data.empty()) {
        taskId = getTaskId(recv_data);
    } 

    switch (taskId)
    {
        case taskId::GetPassWord: {
            int result = getPassWord("data/csi_user.json", recv_data);
            if ( 200 ==  result ) {
                // 创建 JSON 对象
                nlohmann::json jsonObject;
                jsonObject["response"] = "ok";
                jsonObject["errorCode"] = result;

                // 将 JSON 对象序列化为字符串
                std::string jsonString = jsonObject.dump();

                this->send(jsonString);
            } else {
                // 创建 JSON 对象
                nlohmann::json jsonObject;
                jsonObject["response"] = "ok";
                jsonObject["errorCode"] = result;

                // 将 JSON 对象序列化为字符串
                std::string jsonString = jsonObject.dump();

                this->send(jsonString);
            }
            break;
        }
        case taskId::DevTableSave: {
            int result = devTableSave("data/csi_devtable.json", recv_data);
            if ( 200 ==  result ) {
                // 创建 JSON 对象
                nlohmann::json jsonObject;
                jsonObject["response"] = "ok";
                jsonObject["errorCode"] = result;

                // 将 JSON 对象序列化为字符串
                std::string jsonString = jsonObject.dump();

                this->send(jsonString);
            } else {
                // 创建 JSON 对象
                nlohmann::json jsonObject;
                jsonObject["response"] = "ok";
                jsonObject["errorCode"] = result;

                // 将 JSON 对象序列化为字符串
                std::string jsonString = jsonObject.dump();

                this->send(jsonString);
            }
            break;
        }
        case taskId::ChannelTableSave: {
            int result = channelTableSave("data/csi_channeltable.json", recv_data);
            if ( 200 ==  result ) {
                // 创建 JSON 对象
                nlohmann::json jsonObject;
                jsonObject["response"] = "ok";
                jsonObject["errorCode"] = result;

                // 将 JSON 对象序列化为字符串
                std::string jsonString = jsonObject.dump();

                this->send(jsonString);
            } else {
                // 创建 JSON 对象
                nlohmann::json jsonObject;
                jsonObject["response"] = "ok";
                jsonObject["errorCode"] = result;

                // 将 JSON 对象序列化为字符串
                std::string jsonString = jsonObject.dump();

                this->send(jsonString);
            }
            break;
        }
        case taskId::GetDevTable: {
            std::string data;
            int result = tableLoad("data/csi_devtable.json", data);
            if ( 200 ==  result ) {
                // 创建 JSON 对象
                nlohmann::json jsonObject;
                jsonObject["response"] = "ok";
                jsonObject["errorCode"] = result;
                jsonObject["data"] = data;

                // 将 JSON 对象序列化为字符串
                std::string jsonString = jsonObject.dump();

                this->send(jsonString);
            } else {
                // 创建 JSON 对象
                nlohmann::json jsonObject;
                jsonObject["response"] = "ok";
                jsonObject["errorCode"] = result;

                // 将 JSON 对象序列化为字符串
                std::string jsonString = jsonObject.dump();

                this->send(jsonString);
            }

            break;
        }
        case taskId::GetChannelTable: {
            std::string data;
            int result = tableLoad("data/csi_channeltable.json", data);
            if ( 200 ==  result ) {
                // 创建 JSON 对象
                nlohmann::json jsonObject;
                jsonObject["response"] = "ok";
                jsonObject["errorCode"] = result;
                jsonObject["data"] = data;

                // 将 JSON 对象序列化为字符串
                std::string jsonString = jsonObject.dump();

                this->send(jsonString);
            } else {
                // 创建 JSON 对象
                nlohmann::json jsonObject;
                jsonObject["response"] = "ok";
                jsonObject["errorCode"] = result;

                // 将 JSON 对象序列化为字符串
                std::string jsonString = jsonObject.dump();

                this->send(jsonString);
            }

            break;
        }
        
        default:
            break;
    }
}

int NetWorkLayerImp::getTaskId(const std::string recv_data) {
    // 解析JSON数据
    json http_jsonData;
    int http_taskid;
    try {
        http_jsonData = json::parse(recv_data);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        // 获取字段的值
        http_taskid = http_jsonData["taskID"].get<int>();
        LOG_DEBUG("任务id: {}\n", http_taskid);

    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }

    return http_taskid;
}

int NetWorkLayerImp::getPassWord(const std::string filename, const std::string recv_data) {
    // 打开JSON文件
    std::string name;
    int password;
    std::ifstream file(filename);
    if (!file.is_open()) {
        // 文件打开失败，进行相应的处理
        LOG_ERROR("文件打开失败\n");
        return 0;
    } else {
        // 文件成功打开，继续读取文件内容
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        LOG_DEBUG("Json解析: {}\n", content.c_str());

        // 解析JSON数据
        json jsonData;
        try {
            jsonData = json::parse(content);

            // 验证JSON数据的有效性
            if (!jsonData.is_object()) {
                throw std::runtime_error("Invalid JSON data: not an object");
            }

            // 获取字段的值
            name = jsonData["username"].get<std::string>();
            password = jsonData["password"].get<int>();
            LOG_DEBUG("保存的用户名密码：{}/{}\n", name.c_str(), password);

        } catch(const std::exception& e) {
            // Json解析错误
            LOG_ERROR("Json解析错误\n");
            return 0;
        }

        file.close();
    }

    // 解析JSON数据
    json http_jsonData;
    std::string http_name;
    int http_password;
    try {
        http_jsonData = json::parse(recv_data);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        // 获取字段的值
        http_name = http_jsonData["data"]["username"].get<std::string>();
        http_password = http_jsonData["data"]["password"].get<int>();
        LOG_DEBUG("http的用户名密码: {}/{}\n", http_name.c_str(), http_password);

    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }

    // 返回响应给客户端
    if (name == http_name && password == http_password) {
        LOG_ERROR("用户密码正确！\n");
        return 200; // 设置响应状态码
    } else {
        return 400;
    }

    return 1;
}

int NetWorkLayerImp::devTableSave(const std::string filename, const std::string recv_data) {
    // 解析JSON数据
    json http_jsonData;
    try {
        http_jsonData = json::parse(recv_data);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        // 保存到文件
        std::ofstream o(filename.c_str());
        o << http_jsonData.dump(4) << std::endl;  // 使用dump(4)进行格式化，其中4是缩进

        LOG_DEBUG("Json解析正确！\n");
    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }

    return 200;
}

int NetWorkLayerImp::channelTableSave(const std::string filename, const std::string recv_data) {
    // 解析JSON数据
    json http_jsonData;
    try {
        http_jsonData = json::parse(recv_data);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        // 保存到文件
        std::ofstream o(filename.c_str());
        o << http_jsonData.dump(4) << std::endl;  // 使用dump(4)进行格式化，其中4是缩进

        LOG_DEBUG("Json解析正确！\n");
    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }

    return 200;
}

int NetWorkLayerImp::tableLoad(const std::string filename, std::string& data) {
    // 打开JSON文件
    std::ifstream file(filename);
    if (!file.is_open()) {
        // 文件打开失败，进行相应的处理
        LOG_ERROR("文件打开失败\n");
        return 0;
    } else {
        // 文件成功打开，继续读取文件内容
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        LOG_DEBUG("Json解析: {}\n", content.c_str());

        // 解析JSON数据
        json jsonData;
        try {
            jsonData = json::parse(content);

            // 验证JSON数据的有效性
            if (!jsonData.is_object()) {
                throw std::runtime_error("Invalid JSON data: not an object");
            }

            LOG_DEBUG("Json解析正确！\n");
        } catch(const std::exception& e) {
            // Json解析错误
            LOG_ERROR("Json解析错误\n");
            return 0;
        }

        data = content;
        file.close();
    }

    return 200;
}
