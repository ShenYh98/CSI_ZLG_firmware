#pragma once

#include <iostream>
#include <string>
#include <set>

#include <mutex>
#include <condition_variable>

#include "nlohmann/json.hpp"
#include "../common/CommonLog.h"

using json = nlohmann::json;
using namespace CommonLib;

class NetworkService {
public:
    virtual void receive(std::string& data) = 0;
    virtual void send(std::string& data) = 0;
};
