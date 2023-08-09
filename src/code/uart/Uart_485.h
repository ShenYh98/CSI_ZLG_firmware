#pragma once
#include "UartAbstract.h"

namespace UartMiddleware {

typedef struct {
    int baudrate; // 波特率
    int databit;    // 数据为
    std::string stopbit;  // 停止位
    char parity;    // 奇偶性
    int vtime;      // 最小字节
    int vmin;       // 最迟响应时间
} UartParamInfo;

typedef struct {
    int UartId;             // 创建的id,通过打开文件来获取
    int statu;              // 0未使能 1使能
    std::string UartName;   // 驱动文件名
    UartParamInfo uartParamInfo;
} UartIdInfo;

class Uart_485 : public UartAbstract {
public:
    Uart_485(const std::string& jsonData);
    ~Uart_485();

    void receive(char* buf) override;
    void send(const char* buf) override;

private:
    int openDriver(const std::string& data) override;

    int json_get_param(const std::string& jsonData);

    void set_baudrate (struct termios *opt, unsigned int baudrate);
    void set_data_bit (struct termios *opt, unsigned int databit);
    void set_parity (struct termios *opt, char parity);
    void set_stopbit (struct termios *opt, const char *stopbit);
    int  set_port_attr (int fd,int  baudrate, int  databit, const char *stopbit, char parity, int vtime,int vmin );
private:
    UartIdInfo uartIdInfo;
};


}
