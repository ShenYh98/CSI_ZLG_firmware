#include "NetWorkLayerImp.h"

using namespace NetWorkMiddleware;

NetWorkLayerImp::NetWorkLayerImp(NetworkService* networkSrv) {
    _networkSrv = networkSrv;

    // 接收发送的连续寄存器的数据
    MessageQueue< std::pair<std::string, std::vector<s_RTSrcInfo>> >::getInstance().subscribe("TaskSerial/seriesRTSrcInfo", 
        [&](const std::pair<std::string, std::vector<s_RTSrcInfo>>& msg) {
            LOG_DEBUG("rt source len {}\n", msg.second.size());

            v_RTSrcInfo = msg.second;
            sn = msg.first;
        });
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
            int result = getDevTable(data);
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
            int result = getChannelTable(data);
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
        case taskId::AddDev: {
            int result = addDev(recv_data);
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
        case taskId::EditDev: {
            int result = editDev(recv_data);
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
        case taskId::DelDev: {
            int result = delDev(recv_data);
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
        case taskId::AddChannel: {
            int result = addChannel(recv_data);
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
        case taskId::EditChannel: {
            int result = editChannel(recv_data);
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
        case taskId::DelChannel: {
            int result = delChannel(recv_data);
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
        case taskId::GetRTData: {
            int result = getRTData(recv_data);
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
        
        default:
            break;
    }
}

void NetWorkLayerImp::operation(s_RTtask rttask) {
    switch (rttask.id) {
        case RTtaskId::Yc : {
            std::ostringstream payloadStream;
            printf("rttask.dev_sn:%s sn:%s\n", rttask.dev_sn.c_str(), sn.c_str());
            sendDataToWeb(rttask, payloadStream);
            auto str = payloadStream.str();
            this->send(str);

            break;
        }
        default:
            break;
    }
}

void NetWorkLayerImp::sendDataToWeb(const s_RTtask& rttask, std::ostringstream& payloadStream) {
    if (rttask.dev_sn == sn) {
        payloadStream << R"([)";
        for (size_t i = 0; i < v_RTSrcInfo.size(); ++i) {
            if (v_RTSrcInfo[i].value.idata == -1 && v_RTSrcInfo[i].value.sData == "NULL") {
                payloadStream << R"({"sourceName":")" << v_RTSrcInfo[i].sourceName 
                        << R"(", "value":")" << std::to_string(v_RTSrcInfo[i].value.ddata) 
                        << R"(", "unit":")" << v_RTSrcInfo[i].unit << R"("})";
            } else if (v_RTSrcInfo[i].value.ddata == -1.0 && v_RTSrcInfo[i].value.sData == "NULL") {
                payloadStream << R"({"sourceName":")" << v_RTSrcInfo[i].sourceName 
                        << R"(", "value":")" << std::to_string(v_RTSrcInfo[i].value.idata) 
                        << R"(", "unit":")" << v_RTSrcInfo[i].unit << R"("})";
            } else {
                payloadStream << R"({"sourceName":")" << v_RTSrcInfo[i].sourceName 
                        << R"(", "value":")" << v_RTSrcInfo[i].value.sData
                        << R"(", "unit":")" << v_RTSrcInfo[i].unit << R"("})";
            }
            
            if (i != v_RTSrcInfo.size() - 1) {
                payloadStream << R"(,)";
            }
        }
        payloadStream << R"(])";
        
        v_PreRTSrcInfo = v_RTSrcInfo;
    } else if (preSn == rttask.dev_sn) {
        payloadStream << R"([)";
        for (size_t i = 0; i < v_PreRTSrcInfo.size(); ++i) {
            if (v_PreRTSrcInfo[i].value.idata == -1 && v_PreRTSrcInfo[i].value.sData == "NULL") {
                payloadStream << R"({"sourceName":")" << v_PreRTSrcInfo[i].sourceName 
                        << R"(", "value":")" << std::to_string(v_PreRTSrcInfo[i].value.ddata) 
                        << R"(", "unit":")" << v_PreRTSrcInfo[i].unit << R"("})";
            } else if (v_PreRTSrcInfo[i].value.ddata == -1.0 && v_PreRTSrcInfo[i].value.sData == "NULL") {
                payloadStream << R"({"sourceName":")" << v_PreRTSrcInfo[i].sourceName 
                        << R"(", "value":")" << std::to_string(v_PreRTSrcInfo[i].value.idata) 
                        << R"(", "unit":")" << v_PreRTSrcInfo[i].unit << R"("})";
            } else {
                payloadStream << R"({"sourceName":")" << v_PreRTSrcInfo[i].sourceName 
                        << R"(", "value":")" << v_PreRTSrcInfo[i].value.sData
                        << R"(", "unit":")" << v_PreRTSrcInfo[i].unit << R"("})";
            }
            
            if (i != v_PreRTSrcInfo.size() - 1) {
                payloadStream << R"(,)";
            }
        }
        payloadStream << R"(])";
    } else {
        payloadStream << R"([)";
        payloadStream << R"({"sourceName":")" << ""
                    << R"(", "value":")" << ""
                    << R"(", "unit":")" << "" << R"("})";
        payloadStream << R"(])";
    }
    preSn = rttask.dev_sn;
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

int NetWorkLayerImp::getDevTable(std::string& data) {
    std::vector<srv_DevInfo> v_devInfo;
    srv_DevInfo srv_devInfo;

    srv_devInfo.act = Action::Get;
    v_devInfo.push_back(srv_devInfo);

    auto publish = ServiceQueue<std::vector<srv_DevInfo>, srv_GetInfo>::getInstance().service_publish("devTable/addDev", v_devInfo);
    auto res = publish.get();

    if (res.status == "ok") {
        nlohmann::json tableJson = nlohmann::json::array();

        for(const auto& entry : res.DevList) {
            nlohmann::json entryJson;
            entryJson["Id"] = entry.Id;
            entryJson["communicationAddress"] = std::to_string(entry.addr);
            entryJson["category"] = entry.category;
            entryJson["name"] = entry.devName;
            entryJson["model"] = entry.model;
            entryJson["protocol"] = entry.protocol;
            entryJson["port"] = entry.serialInfo.name;
            entryJson["sn"] = entry.sn;

            tableJson.push_back(entryJson);
        }

        // 创建一个包含表的json对象
        nlohmann::json j;
        j["data"]["table"] = tableJson;
        data = j.dump(4);

        return 200;
    } else {
        return 0;
    }

    return 200;
}

int NetWorkLayerImp::addDev(const std::string recv_data) {
    // 解析JSON数据
    json http_jsonData;
    std::vector<srv_DevInfo> v_devInfo;
    try {
        http_jsonData = json::parse(recv_data);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        // 获取字段的值
        srv_DevInfo srv_devInfo;
        srv_devInfo.act = Action::Add;
        srv_devInfo.devInfo.addr = std::stoi(http_jsonData["data"]["communicationAddress"].get<std::string>());
        srv_devInfo.devInfo.category = http_jsonData["data"]["category"].get<std::string>();
        srv_devInfo.devInfo.devName = http_jsonData["data"]["name"].get<std::string>();
        srv_devInfo.devInfo.model = http_jsonData["data"]["model"].get<std::string>();
        srv_devInfo.devInfo.protocol = http_jsonData["data"]["protocol"].get<std::string>();
        srv_devInfo.devInfo.serialInfo.name = http_jsonData["data"]["port"].get<std::string>();
        srv_devInfo.devInfo.sn = http_jsonData["data"]["sn"].get<std::string>();
        LOG_DEBUG("dev action: {}\n", srv_devInfo.act);
        LOG_DEBUG("dev Id: {}\n", srv_devInfo.devInfo.Id);
        LOG_DEBUG("dev addr: {}\n", srv_devInfo.devInfo.addr);
        LOG_DEBUG("dev category: {}\n", srv_devInfo.devInfo.category);
        LOG_DEBUG("dev devName: {}\n", srv_devInfo.devInfo.devName);
        LOG_DEBUG("dev model: {}\n", srv_devInfo.devInfo.model);
        LOG_DEBUG("dev protocol: {}\n", srv_devInfo.devInfo.protocol);
        LOG_DEBUG("dev serialInfo: {}\n", srv_devInfo.devInfo.serialInfo.name);
        LOG_DEBUG("dev sn: {}\n", srv_devInfo.devInfo.sn);

        v_devInfo.push_back(srv_devInfo);
    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }

    auto publish = ServiceQueue<std::vector<srv_DevInfo>, srv_GetInfo>::getInstance().service_publish("devTable/addDev", v_devInfo);
    auto res = publish.get();
    if (res.status == "ok") {
        return 200;
    } else {
        return 0;
    }

    return 200;
}

int NetWorkLayerImp::editDev(const std::string recv_data) {
    // 解析JSON数据
    json http_jsonData;
    std::vector<srv_DevInfo> v_devInfo;
    try {
        http_jsonData = json::parse(recv_data);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        // 获取字段的新值
        srv_DevInfo srv_devInfo;

        srv_devInfo.act = Action::Edit;
        srv_devInfo.devInfo.Id = http_jsonData["data"]["Id"].get<std::string>();
        srv_devInfo.devInfo.addr = std::stoi(http_jsonData["data"]["communicationAddress"].get<std::string>());
        srv_devInfo.devInfo.category = http_jsonData["data"]["category"].get<std::string>();
        srv_devInfo.devInfo.devName = http_jsonData["data"]["name"].get<std::string>();
        srv_devInfo.devInfo.model = http_jsonData["data"]["model"].get<std::string>();
        srv_devInfo.devInfo.protocol = http_jsonData["data"]["protocol"].get<std::string>();
        srv_devInfo.devInfo.serialInfo.name = http_jsonData["data"]["port"].get<std::string>();
        srv_devInfo.devInfo.sn = http_jsonData["data"]["sn"].get<std::string>();

        LOG_DEBUG("dev old action: {}\n", srv_devInfo.act);
        LOG_DEBUG("dev old Id: {}\n", srv_devInfo.devInfo.Id);
        LOG_DEBUG("dev old addr: {}\n", srv_devInfo.devInfo.addr);
        LOG_DEBUG("dev old category: {}\n", srv_devInfo.devInfo.category);
        LOG_DEBUG("dev old devName: {}\n", srv_devInfo.devInfo.devName);
        LOG_DEBUG("dev old model: {}\n", srv_devInfo.devInfo.model);
        LOG_DEBUG("dev old protocol: {}\n", srv_devInfo.devInfo.protocol);
        LOG_DEBUG("dev old serialInfo: {}\n", srv_devInfo.devInfo.serialInfo.name);
        LOG_DEBUG("dev old sn: {}\n", srv_devInfo.devInfo.sn);

        v_devInfo.push_back(srv_devInfo);
    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误: {}\n", e.what());
        return 0;
    }

    auto publish = ServiceQueue<std::vector<srv_DevInfo>, srv_GetInfo>::getInstance().service_publish("devTable/addDev", v_devInfo);
    auto res = publish.get();
    if (res.status == "ok") {
        return 200;
    } else {
        return 0;
    }

    return 200;
}

int NetWorkLayerImp::delDev(const std::string recv_data) {
    // 解析JSON数据
    json http_jsonData;
    std::vector<srv_DevInfo> v_devInfo;
    try {
        http_jsonData = json::parse(recv_data);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        // 获取字段的值
        srv_DevInfo srv_devInfo;
        srv_devInfo.act = Action::Del;
        srv_devInfo.devInfo.Id = http_jsonData["data"]["Id"].get<std::string>();
        srv_devInfo.devInfo.addr = std::stoi(http_jsonData["data"]["communicationAddress"].get<std::string>());
        srv_devInfo.devInfo.category = http_jsonData["data"]["category"].get<std::string>();
        srv_devInfo.devInfo.devName = http_jsonData["data"]["name"].get<std::string>();
        srv_devInfo.devInfo.model = http_jsonData["data"]["model"].get<std::string>();
        srv_devInfo.devInfo.protocol = http_jsonData["data"]["protocol"].get<std::string>();
        srv_devInfo.devInfo.serialInfo.name = http_jsonData["data"]["port"].get<std::string>();
        srv_devInfo.devInfo.sn = http_jsonData["data"]["sn"].get<std::string>();

        v_devInfo.push_back(srv_devInfo);
    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }

    auto publish = ServiceQueue< std::vector<srv_DevInfo>, srv_GetInfo >::getInstance().service_publish("devTable/addDev", v_devInfo);
    auto res = publish.get();
    if (res.status == "ok") {
        return 200;
    } else {
        return 0;
    }

    return 200;
}

int NetWorkLayerImp::getChannelTable(std::string& data) {
    std::vector<srv_SerialInfo> v_serialInfo;
    srv_SerialInfo srv_serialInfo;

    srv_serialInfo.act = Action::Get;
    v_serialInfo.push_back(srv_serialInfo);

    auto publish = ServiceQueue<std::vector<srv_SerialInfo>, srv_GetInfo>::getInstance().service_publish("SerialTable/addSerial", v_serialInfo);
    auto res = publish.get();

    if (res.status == "ok") {
        nlohmann::json tableJson = nlohmann::json::array();

        // 遍历C++数组并将每个元素添加到json数组中
        for(const auto& entry : res.SerialList) {
            nlohmann::json entryJson;
            entryJson["Id"] = entry.Id;
            entryJson["baudRate"] = std::to_string(entry.serialParamInfo.baudrate);
            entryJson["dataBits"] = std::to_string(entry.serialParamInfo.databit);
            entryJson["parity"] = entry.serialParamInfo.parity;
            entryJson["port"] = entry.SerialName;
            entryJson["portName"] = entry.name;
            entryJson["stopBits"] = entry.serialParamInfo.stopbit;
            entryJson["vmin"] = entry.serialParamInfo.vmin;
            entryJson["vtime"] = entry.serialParamInfo.vtime;

            tableJson.push_back(entryJson);
        }

        // 创建一个包含表的json对象
        nlohmann::json j;
        j["data"]["table"] = tableJson;
        data = j.dump(4);

        return 200;
    } else {
        return 0;
    }

    return 200;
}

int NetWorkLayerImp::addChannel(const std::string recv_data) {
    // 解析JSON数据
    json http_jsonData;
    std::vector<srv_SerialInfo> v_serialInfo;
    try {
        http_jsonData = json::parse(recv_data);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        // 获取字段的值
        srv_SerialInfo srv_serialInfo;

        srv_serialInfo.act = Action::Add;
        srv_serialInfo.serialInfo.name = http_jsonData["data"]["portName"].get<std::string>();
        srv_serialInfo.serialInfo.SerialName = http_jsonData["data"]["port"].get<std::string>();
        if (http_jsonData["data"]["baudRate"].is_string()) {
            srv_serialInfo.serialInfo.serialParamInfo.baudrate = std::atoi(http_jsonData["data"]["baudRate"].get<std::string>().c_str());
        } else {
            srv_serialInfo.serialInfo.serialParamInfo.baudrate = http_jsonData["data"]["baudRate"].get<int>();
        }
        if (http_jsonData["data"]["dataBits"].is_string()) {
            srv_serialInfo.serialInfo.serialParamInfo.databit = std::atoi(http_jsonData["data"]["dataBits"].get<std::string>().c_str());
        } else {
            srv_serialInfo.serialInfo.serialParamInfo.databit = http_jsonData["data"]["dataBits"].get<int>();
        }
        srv_serialInfo.serialInfo.serialParamInfo.parity = http_jsonData["data"]["parity"].get<std::string>();
        srv_serialInfo.serialInfo.serialParamInfo.stopbit = http_jsonData["data"]["stopBits"].get<std::string>();
        srv_serialInfo.serialInfo.serialParamInfo.vmin = 1;
        srv_serialInfo.serialInfo.serialParamInfo.vtime = 15;

        v_serialInfo.push_back(srv_serialInfo);
    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }

    auto publish = ServiceQueue<std::vector<srv_SerialInfo>, srv_GetInfo>::getInstance().service_publish("SerialTable/addSerial", v_serialInfo);
    auto res = publish.get();
    if (res.status == "ok") {
        return 200;
    } else {
        return 0;
    }

    return 200;
}

int NetWorkLayerImp::editChannel(const std::string recv_data) {
    // 解析JSON数据
    json http_jsonData;
    std::vector<srv_SerialInfo> v_serialInfo;
    try {
        http_jsonData = json::parse(recv_data);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        // 获取字段的新值
        srv_SerialInfo srv_serialInfo;

        srv_serialInfo.act = Action::Edit;
        srv_serialInfo.serialInfo.name                      = http_jsonData["data"]["portName"].get<std::string>();
        srv_serialInfo.serialInfo.Id                        = http_jsonData["data"]["Id"].get<std::string>();
        srv_serialInfo.serialInfo.SerialName                = http_jsonData["data"]["port"].get<std::string>();
        srv_serialInfo.serialInfo.serialParamInfo.baudrate  = std::atoi(http_jsonData["data"]["baudRate"].get<std::string>().c_str());
        srv_serialInfo.serialInfo.serialParamInfo.databit   = std::atoi(http_jsonData["data"]["dataBits"].get<std::string>().c_str());
        srv_serialInfo.serialInfo.serialParamInfo.parity    = http_jsonData["data"]["parity"].get<std::string>();
        srv_serialInfo.serialInfo.serialParamInfo.stopbit   = http_jsonData["data"]["stopBits"].get<std::string>();
        srv_serialInfo.serialInfo.serialParamInfo.vmin      = 1;
        srv_serialInfo.serialInfo.serialParamInfo.vtime     = 15;

        LOG_DEBUG("serial old action: {}\n",    srv_serialInfo.act);
        LOG_DEBUG("serial old name: {}\n",      srv_serialInfo.serialInfo.name                     );
        LOG_DEBUG("serial old baudrate: {}\n",  srv_serialInfo.serialInfo.serialParamInfo.baudrate );
        LOG_DEBUG("serial old databit: {}\n",   srv_serialInfo.serialInfo.serialParamInfo.databit  );
        LOG_DEBUG("serial old parity: {}\n",    srv_serialInfo.serialInfo.serialParamInfo.parity   );
        LOG_DEBUG("serial old stopbit: {}\n",   srv_serialInfo.serialInfo.serialParamInfo.stopbit  );
        LOG_DEBUG("serial old vmin: {}\n",      srv_serialInfo.serialInfo.serialParamInfo.vmin     );
        LOG_DEBUG("serial old vtime: {}\n",     srv_serialInfo.serialInfo.serialParamInfo.vtime    );

        v_serialInfo.push_back(srv_serialInfo);
    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误: {}\n", e.what());
        return 0;
    }

    auto publish = ServiceQueue<std::vector<srv_SerialInfo>, srv_GetInfo>::getInstance().service_publish("SerialTable/addSerial", v_serialInfo);
    auto res = publish.get();
    if (res.status == "ok") {
        return 200;
    } else {
        return 0;
    }

    return 200;
}

int NetWorkLayerImp::delChannel(const std::string recv_data) {
    // 解析JSON数据
    json http_jsonData;
    std::vector<srv_SerialInfo> v_serialInfo;
    try {
        http_jsonData = json::parse(recv_data);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        // 获取字段的值
        srv_SerialInfo srv_serialInfo;
        srv_serialInfo.act = Action::Del;
        srv_serialInfo.serialInfo.Id = http_jsonData["data"]["Id"].get<std::string>();
        srv_serialInfo.serialInfo.name = http_jsonData["data"]["portName"].get<std::string>();
        srv_serialInfo.serialInfo.SerialName = http_jsonData["data"]["port"].get<std::string>();
        srv_serialInfo.serialInfo.serialParamInfo.baudrate = std::atoi(http_jsonData["data"]["baudRate"].get<std::string>().c_str());
        srv_serialInfo.serialInfo.serialParamInfo.databit = std::atoi(http_jsonData["data"]["dataBits"].get<std::string>().c_str());
        srv_serialInfo.serialInfo.serialParamInfo.parity = http_jsonData["data"]["parity"].get<std::string>();
        srv_serialInfo.serialInfo.serialParamInfo.stopbit = http_jsonData["data"]["stopBits"].get<std::string>();
        srv_serialInfo.serialInfo.serialParamInfo.vmin = 1;
        srv_serialInfo.serialInfo.serialParamInfo.vtime = 15;

        v_serialInfo.push_back(srv_serialInfo);
    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }

    auto publish = ServiceQueue<std::vector<srv_SerialInfo>, srv_GetInfo>::getInstance().service_publish("SerialTable/addSerial", v_serialInfo);
    auto res = publish.get();
    if (res.status == "ok") {
        return 200;
    } else {
        return 0;
    }

    return 200;
}

int NetWorkLayerImp::getRTData(const std::string recv_data) {
    // 解析JSON数据
    json http_jsonData;

    s_RTtask rttask;

    try {
        http_jsonData = json::parse(recv_data);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        auto tmpstr = http_jsonData["data"]["RTtaskId"].get<std::string>();
        // 获取字段的值
        if ( tmpstr == "Yc") {
            rttask.id = RTtaskId::Yc;
        } else if ( tmpstr == "Yk") {
            rttask.id = RTtaskId::Yk;
        } else if ( tmpstr == "Yt") {
            rttask.id = RTtaskId::Yt;
        } else if ( tmpstr == "Yx") {
            rttask.id = RTtaskId::Yx;
        }
        rttask.isOn = http_jsonData["data"]["isOn"].get<int>();
        rttask.dev_sn = http_jsonData["data"]["dev_sn"].get<std::string>();

        LOG_DEBUG("id: {} / isOn: {} / sn: {} \n",rttask.id ,rttask.isOn ,rttask.dev_sn);

        MessageQueue<s_RTtask>::getInstance().publish("TaskWebSocket/getRtData", rttask);

    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }

    return 200;
}
