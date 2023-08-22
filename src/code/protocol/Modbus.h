#pragma once

#include "Protocol.h"

namespace ProtocolMiddleware {

class Modbus : public Protocol {
public:
    // 以何种网络协议通信
    Modbus(SerialAbstract* serial);
    ~Modbus();

    virtual void ParsePacket(const uint8_t* request) override;
    virtual void AssemblePacket(uint8_t* request) override;

    virtual void receive(char* buf) override;
    virtual void send(const char* buf) override;

private:
    //CRC校验码公式
    uint16_t CRC16(uint8_t* pDataBuf, int DataLen);

private:
    SerialAbstract* _serial;
};

}