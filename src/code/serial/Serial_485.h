#pragma once
#include "SerialAbstract.h"

namespace SerialMiddleware {

class Serial_485 : public SerialAbstract {
public:
    Serial_485(const std::string& jsonData);
    ~Serial_485();

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
    SerialIdInfo serialIdInfo;
};


}
