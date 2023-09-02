#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "nlohmann/json.hpp"
#include "../common/CommonLog.h"

#include "../../DataStructure.h"

using json = nlohmann::json;
using namespace CommonLib;

class SerialAbstract {
public:
    virtual int receive(char* buf) = 0;
    virtual int send(const char* buf) = 0;

private:
    virtual int openDriver(const std::string& data) = 0;
};
