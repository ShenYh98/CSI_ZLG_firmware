#pragma once

#include <iostream>

#include "../common/MessageQueue.hpp"
#include "../common/CommonLog.h"

#include "../../DataStructure.h"

#include "nlohmann/json.hpp"

using namespace CommonLib;
using json = nlohmann::json;

namespace SettingMiddleware {

class Setting
{
public:
    Setting();
    ~Setting();

private:
    void srvDevActionTask(const std::vector<srv_DevInfo>& msg, std::function<void(const std::string&)> responder);

    void saveDevJson(const std::string& fileName);

private:
    std::vector<DevInfo> devList;

    std::string addDevPath;

};

}



