#include "Modbus.h"

using namespace ProtocolMiddleware;

Modbus::Modbus(SerialAbstract* serial) {
    _serial = serial;
}

Modbus::~Modbus() {
}

void Modbus::ParsePacket(const uint8_t* request) {
}

void Modbus::AssemblePacket(uint8_t* request) {
    int request_len;

    // 构建Modbus RTU读取请求
    request[0] = 0x01;  // 设备地址
    request[1] = 0x03;  // 功能码
    request[2] = 0x0b;  // 起始地址高位
    request[3] = 0x24;  // 起始地址低位
    request[4] = 0x00;  // 寄存器数量高位
    request[5] = 0x01;  // 寄存器数量低位

    // 使用libmodbus添加CRC校验
    uint16_t crc = CRC16(request, 6);
    request[6] = (crc & 0xFF);
    request[7] = (crc >> 8);
}

void Modbus::receive(char* buf) {
    _serial->receive(buf);
}

void Modbus::send(const char* buf) {
    _serial->send(buf);
}

uint16_t Modbus::CRC16(uint8_t* pDataBuf, int DataLen) {
    int i = 0;
    int j = 0;
    uint16_t crc = 0xFFFF;		// 赋初值

    for ( i = 0; i < DataLen; i++ ) {
        crc = crc ^ (uint16_t)pDataBuf[i];
        for ( j = 0; j < 8; j++ ) {
            if ( ( crc & 0x0001 ) != 0 ) {
                crc = ( crc >> 1 ) ^ 0xA001;		// CRC校验特征公式
            } else {
                crc = crc >> 1;
            }
        }// End of for(j)
    }// End of for(i)

    return crc;
}
