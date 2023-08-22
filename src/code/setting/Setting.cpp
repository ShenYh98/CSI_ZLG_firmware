#include "Setting.h"

using namespace SettingMiddleware;

Setting::Setting(/* args */) {
    addDevPath = "data/csi_devtable.json";

    ServiceQueue< std::vector<srv_DevInfo>, std::string >::getInstance().service_subscribe("devTable/addDev", 
                    std::bind(&Setting::srvDevActionTask, this, std::placeholders::_1, std::placeholders::_2));

}

Setting::~Setting()
{
}

void Setting::srvDevActionTask(const std::vector<srv_DevInfo>& msg, std::function<void(const std::string&)> responder) {
    LOG_DEBUG("msg action: {}\n", msg[0].act);
    int res = 0;

    switch (msg[0].act)
    {
    case Action::Add : {
        LOG_DEBUG("msg add addr: {}\n", msg[0].devInfo.addr);
        LOG_DEBUG("msg add category: {}\n", msg[0].devInfo.category);
        LOG_DEBUG("msg add deviceStatus: {}\n", msg[0].devInfo.deviceStatus);
        LOG_DEBUG("msg add devName: {}\n", msg[0].devInfo.devName);
        LOG_DEBUG("msg add model: {}\n", msg[0].devInfo.model);
        LOG_DEBUG("msg add protocol: {}\n", msg[0].devInfo.protocol);
        LOG_DEBUG("msg add serialInfo: {}\n", msg[0].devInfo.serialInfo.name);
        LOG_DEBUG("msg add sn: {}\n", msg[0].devInfo.sn);

        auto device = msg[0].devInfo;
        devList.push_back(device);
        saveDevJson(addDevPath);

        res = 1;

        break;
    }
    case Action::Edit : {
        LOG_DEBUG("msg edit addr: {}\n", msg[0].devInfo.addr);
        LOG_DEBUG("msg edit category: {}\n", msg[0].devInfo.category);
        LOG_DEBUG("msg edit deviceStatus: {}\n", msg[0].devInfo.deviceStatus);
        LOG_DEBUG("msg edit devName: {}\n", msg[0].devInfo.devName);
        LOG_DEBUG("msg edit model: {}\n", msg[0].devInfo.model);
        LOG_DEBUG("msg edit protocol: {}\n", msg[0].devInfo.protocol);
        LOG_DEBUG("msg edit serialInfo: {}\n", msg[0].devInfo.serialInfo.name);
        LOG_DEBUG("msg edit sn: {}\n", msg[0].devInfo.sn);  
        
        LOG_DEBUG("old msg edit addr: {}\n", msg[0].oldDevInfo.addr);
        LOG_DEBUG("old msg edit category: {}\n", msg[0].oldDevInfo.category);
        LOG_DEBUG("old msg edit deviceStatus: {}\n", msg[0].oldDevInfo.deviceStatus);
        LOG_DEBUG("old msg edit devName: {}\n", msg[0].oldDevInfo.devName);
        LOG_DEBUG("old msg edit model: {}\n", msg[0].oldDevInfo.model);
        LOG_DEBUG("old msg edit protocol: {}\n", msg[0].oldDevInfo.protocol);
        LOG_DEBUG("old msg edit serialInfo: {}\n", msg[0].oldDevInfo.serialInfo.name);
        LOG_DEBUG("old msg edit sn: {}\n", msg[0].oldDevInfo.sn);

        auto device = msg[0].devInfo;
        auto olddevice = msg[0].oldDevInfo;
        auto it = std::find_if(devList.begin(), devList.end(),
        [&olddevice](const DevInfo& dev) {
            return dev.devName == olddevice.devName;
        });
        if (it != devList.end())
        {
            *it = device;
            res = 1;
        }
        else
        {
            res = 0;
        }
        saveDevJson(addDevPath);

        break;
    }
    case Action::Del : {
        for (auto it : msg) {
            auto device = it.devInfo; 
            auto findDel = std::find_if(devList.begin(), devList.end(),
                                        [&device](const DevInfo& dev) {
                                            return dev.devName == device.devName;
                                        });
            if (findDel != devList.end())
            {
                devList.erase(findDel);
                res = 1;
            }
            else
            {
                res = 0;
            }
        }

        saveDevJson(addDevPath); 

        break;
    }

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
        j_dev["communicationAddress"] = std::to_string(dev.addr);
        j_dev["category"] = dev.category;
        j_dev["model"] = dev.model;
        j_dev["sn"] = dev.sn;
        j_dev["deviceStatus"] = std::to_string(dev.deviceStatus);

        devArray.push_back(j_dev);
    }

    j["data"]["table"] = devArray;

    // 打开文件并保存json数组
    std::ofstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return;
    }
    file << j.dump(4);  // 使用4个空格进行格式化
    file.close();
}
