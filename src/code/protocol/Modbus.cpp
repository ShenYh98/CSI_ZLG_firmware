#include "Modbus.h"

using namespace ProtocolMiddleware;

Modbus::Modbus(SerialAbstract* serial) {
    pointJsonPath = "data/csi_pcs_modbus.json";
    nextPkg = 0;
    
    _serial = serial;

    loadPointFromJson(pointJsonPath);
}

Modbus::~Modbus() {
}

int Modbus::ParsePacket(uint8_t* request, int length) {
    if (length < 4) {
        LOG_ERROR("[ParsePacket] length < 4\n");
        return 0;
    }

    uint8_t *temp;
    std::copy(request,request+length-2,temp);
    uint16_t calculatedCrc;
    
    calculatedCrc = CRC16(temp,length-2);

    int parse_len;

    if ( calculatedCrc == ( request[length - 2]  << 8 | (request[length - 1]) ) ) {
        parse_len = parseFunc(request, length);
        if (parse_len) {
            LOG_ERROR("[ParsePacket] parse function success\n");
        } else {
            LOG_ERROR("[ParsePacket] parse function error\n");
            return 0;
        }
    } else {  
        LOG_ERROR("[ParsePacket] parse crc error\n");
        return 0;
    } 

    return parse_len;
}

int Modbus::AssemblePacket(uint8_t* request) {
    int len = -1;
    if ( nextPkg < v_point.size() ) {
        // 构建Modbus RTU读取请求
        request[++len] = 0x01;  // 设备地址
        request[++len] = v_point[nextPkg].mpkg[0].func;  // 功能码
        request[++len] = v_point[nextPkg].mpkg[0].reg[0];  // 起始地址高位
        request[++len] = v_point[nextPkg].mpkg[0].reg[1];  // 起始地址低位
        request[++len] = v_point[nextPkg].mpkg[0].len[0];  // 寄存器数量高位
        request[++len] = v_point[nextPkg].mpkg[0].len[1];  // 寄存器数量低位

        // 使用libmodbus添加CRC校验
        uint16_t crc = CRC16(request, 6); 
        request[++len] = (crc & 0xFF);
        request[++len] = (crc >> 8);

        nextPkg++;
    } else {
        len = -1;
        nextPkg = 0;
    }

    return len;
}

int Modbus::receive(char* buf) {
    int len = 0;
    len = _serial->receive(buf);

    return len-1;
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

int Modbus::loadPointFromJson(const std::string& path) {
    // 打开JSON文件
    std::ifstream file(path);
    if (!file.is_open()) {
        // 文件打开失败，进行相应的处理
        LOG_ERROR("文件打开失败\n");
        return 0;
    } else {
        // 文件成功打开，继续读取文件内容
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        LOG_DEBUG("Json解析: {}\n", content.c_str());

        // 解析JSON数据
        json jsonData;
        try {
            jsonData = json::parse(content);

            // 验证JSON数据的有效性
            if (!jsonData.is_object()) {
                throw std::runtime_error("Invalid JSON data: not an object");
            }

            // 获取字段的值
            for (auto item : jsonData["points"]) {
                modbusPoint point;
                point.name = item["name"].get<std::string>();
                point.description = item["description"].get<std::string>();
                point.dataType = item["dataType"].get<std::string>();
                point.res = item["res"].get<int>();
                point.invalid = item["invalid"].get<int>();

                for (auto pkgStringJson : item["mpkg"]) {
                    std::string pkgString = pkgStringJson.get<std::string>();

                    modbusPkg pkg;
                    std::istringstream iss(pkgString);
                    std::string byte;
                    std::vector<uint8_t> bytes;
                    
                    while (std::getline(iss, byte, ',')) {
                        bytes.push_back(std::stoul(byte, nullptr, 16));
                    }
                    
                    pkg.func = bytes[0];
                    pkg.reg = new uint8_t[2]{bytes[1], bytes[2]};
                    pkg.len = new uint8_t[2]{bytes[3], bytes[4]};  // 这里你可能需要根据你的数据来修改
                    
                    point.mpkg.push_back(pkg);
                }

                v_point.push_back(point);
            }
        } catch(const std::exception& e) {
            // Json解析错误
            LOG_ERROR("Json解析错误\n");
            return 0;
        }

        file.close();
    }

    return 1;
}

int Modbus::parseFunc(uint8_t* request, int length) {
    uint8_t deviceId;
    uint8_t functionCode;
    uint8_t dataLength;

    deviceId = request[0];
    functionCode = request[1];
    if (0x80 == ( functionCode & 0x80 ) ) {
        return 0;   
    }

    dataLength = request[2];
    if (dataLength + 5 != length) {
        return 0;
    }

    if (0x03 == functionCode || 0x04 == functionCode) { // 遥测
        //直接修改传入进来的request，即将实际数据传出
        std::copy(request+3,request+3+dataLength,request);
    } else if (0x06 == functionCode) { // 遥调
    }

    return dataLength;
}
