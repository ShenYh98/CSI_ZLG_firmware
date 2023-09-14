#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

// #include <modbus/modbus.h>

#include "nlohmann/json.hpp"

#include "../serial/SerialAbstract.h"
#include "../common/MessageQueue.hpp"

using json = nlohmann::json;

class Protocol {
public:
    virtual int ParsePacket(uint8_t* request, int length) = 0;
    // modbus的默认组包模式是rtu模式
    virtual int AssemblePacket(uint8_t* request) = 0;

    // virtual int receive(char* buf) = 0;
    virtual int receive(uint8_t* buf) = 0;
    virtual void send(const char* buf) = 0;
};
