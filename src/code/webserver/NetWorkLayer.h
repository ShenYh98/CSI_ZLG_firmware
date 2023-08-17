#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include "../common/CommonLog.h"
#include "../common/MessageQueue.hpp"

#include "../../DataStructure.h"

using namespace CommonLib;

class NetWorkLayer {
public:
    virtual void operation() = 0;
    virtual void operation(RTtaskId rttaskId) = 0;
private:
    virtual void receive(std::string& data) = 0;
    virtual void send(std::string& data) = 0;
};
