#pragma once

#include <iostream>

// #include <modbus/modbus.h>

#include "../serial/SerialAbstract.h"

class Protocol {
public:
    virtual void ParsePacket(const uint8_t* request) = 0;
    // modbus的默认组包模式是rtu模式
    virtual void AssemblePacket(uint8_t* request) = 0;

    virtual void receive(char* buf) = 0;
    virtual void send(const char* buf) = 0;
};
