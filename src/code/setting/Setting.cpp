#include "Setting.h"

using namespace SettingMiddleware;

Setting::Setting(/* args */) {
    addDevPath = "data/csi_devtable.json";

    ServiceQueue<srv_DevInfo, std::string>::getInstance().service_subscribe("devTable/addDev", 
                    std::bind(&Setting::srvDevActionTask, this, std::placeholders::_1, std::placeholders::_2));

}

Setting::~Setting()
{
}

void Setting::srvDevActionTask(const srv_DevInfo& msg, std::function<void(const std::string&)> responder) {
    LOG_DEBUG("msg action: {}\n", msg.act);
    int res = 0;

    switch (msg.act)
    {
    case Action::Add : {
        /* code */
        LOG_DEBUG("msg addr: {}\n", msg.devInfo.addr);
        LOG_DEBUG("msg category: {}\n", msg.devInfo.category);
        LOG_DEBUG("msg deviceStatus: {}\n", msg.devInfo.deviceStatus);
        LOG_DEBUG("msg devName: {}\n", msg.devInfo.devName);
        LOG_DEBUG("msg model: {}\n", msg.devInfo.model);
        LOG_DEBUG("msg protocol: {}\n", msg.devInfo.protocol);
        LOG_DEBUG("msg serialInfo: {}\n", msg.devInfo.serialInfo.name);
        LOG_DEBUG("msg sn: {}\n", msg.devInfo.sn);

        auto device = msg.devInfo;
        devList.push_back(device);
        saveDevJson(addDevPath);

        res = 1;

        break;
    }
    case Action::Edit :
        /* code */
        break;

    case Action::Del :
        /* code */
        break;
    
    default:
        break;
    }
    
    if (1 == res) {
        responder("ok");
    } else {
        responder("fail");
    }
    
}

void Setting::saveDevJson(const std::string& fileName) {
    // 创建一个json
    json j;

    // 创建一个json数组
    json devArray = json::array();

    for (auto dev : devList) {
        json j_dev;
        j_dev["name"] = dev.devName;
        j_dev["port"] = dev.serialInfo.name;
        j_dev["protocol"] = dev.protocol;
        j_dev["communicationAddress"] = dev.addr;
        j_dev["category"] = dev.category;
        j_dev["model"] = dev.model;
        j_dev["sn"] = dev.sn;
        j_dev["deviceStatus"] = dev.deviceStatus;

        devArray.push_back(j_dev);
    }

    j["data"]["table"] = devArray;

    // 打开文件并保存json数组
    std::ofstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return;
    }
    file << devArray.dump(4);  // 使用4个空格进行格式化
    file.close();
}
