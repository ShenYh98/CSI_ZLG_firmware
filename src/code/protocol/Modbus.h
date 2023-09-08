#pragma once

#include "Protocol.h"

namespace ProtocolMiddleware {

class Modbus : public Protocol {
public:
    // 以何种网络协议通信
    Modbus(SerialAbstract* serial);
    ~Modbus();

    virtual int ParsePacket(uint8_t* request, int length) override;
    virtual int AssemblePacket(uint8_t* request) override;

    virtual int receive(char* buf) override;
    virtual void send(const char* buf) override;

private:
    //CRC校验码公式
    uint16_t CRC16(uint8_t* pDataBuf, int DataLen);
    uint16_t ReverseCRC16(uint8_t* pDataBuf, int DataLen);

    int loadPointFromJson(const std::string& path);
    int parseFunc(uint8_t* request, int length);

private:
    SerialAbstract* _serial;
    std::vector<modbusPoint> v_point;
    std::string pointJsonPath;
    int nextPkg;
};

}