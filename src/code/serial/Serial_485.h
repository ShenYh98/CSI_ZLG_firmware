#pragma once
#include "SerialAbstract.h"

namespace SerialMiddleware {

class Serial_485 : public SerialAbstract {
public:
    Serial_485(SerialIdInfo& serialIdInfo);
    ~Serial_485();

    int receive(char* buf) override;
    int send(const char* buf) override;

private:
    int openDriver(const std::string& data) override;

    void set_baudrate (struct termios *opt, unsigned int baudrate);
    void set_data_bit (struct termios *opt, unsigned int databit);
    void set_parity (struct termios *opt, std::string parity);
    void set_stopbit (struct termios *opt, const char *stopbit);
    int  set_port_attr (int fd,int  baudrate, int  databit, const char *stopbit, std::string parity, int vtime,int vmin );

private:
    SerialIdInfo serialIdtmp;
};


}
