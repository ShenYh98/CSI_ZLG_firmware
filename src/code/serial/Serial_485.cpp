#include "Serial_485.h"

using namespace SerialMiddleware;

Serial_485::Serial_485(SerialIdInfo& serialIdInfo) {
    std::string serialName;

    if (serialIdInfo.SerialName == "RS485-1") {
        serialName = "/dev/ttyRS485-1";
    } else if (serialIdInfo.SerialName == "RS485-2") {
        serialName = "/dev/ttyRS485-2";
    } else if (serialIdInfo.SerialName == "RS485-3") {
        serialName = "/dev/ttyRS485-3";
    } else if (serialIdInfo.SerialName == "RS485-4") {
        serialName = "/dev/ttyRS485-4";
    } else if (serialIdInfo.SerialName == "RS485-5") {
        serialName = "/dev/ttyRS485-5";
    } else if (serialIdInfo.SerialName == "RS485-6") {
        serialName = "/dev/ttyRS485-6";
    } else if (serialIdInfo.SerialName == "RS485-7") {
        serialName = "/dev/ttyRS485-7";
    } else if (serialIdInfo.SerialName == "RS485-8") {
        serialName = "/dev/ttyRS485-8";
    }

    serialIdInfo.SerialId = openDriver(serialName);
    serialIdInfo.statu = 0;
    set_port_attr(serialIdInfo.SerialId, 
                  serialIdInfo.serialParamInfo.baudrate,
                  serialIdInfo.serialParamInfo.databit,
                  serialIdInfo.serialParamInfo.stopbit.c_str(),
                  serialIdInfo.serialParamInfo.parity,
                  serialIdInfo.serialParamInfo.vtime,
                  serialIdInfo.serialParamInfo.vmin);

    serialIdtmp = serialIdInfo;
}

Serial_485::~Serial_485() {
}

void Serial_485::receive(char* buf) {
    int len;
    len = read(serialIdtmp.SerialId, buf, sizeof(buf));                    /* 在串口读入字符串 */
    if (len < 0) {
        LOG_ERROR("read error \n");
    }
    LOG_DEBUG("recv buf: {}\n", buf);
}
void Serial_485::send(const char* buf) {
    int len;

    len = write(serialIdtmp.SerialId, buf, sizeof(buf));  // 串口写入字符串
    if (len < 0) {
        LOG_ERROR("write data error \n");
    }
}
int Serial_485::openDriver(const std::string& data) {
    int fd;
    fd = open(data.c_str(), O_RDWR | O_NOCTTY);
    if (fd < 0) { // 判断接口是否打开成功
        LOG_ERROR("open Serial device error\n");

        return -1;
    }

    return fd;
}

void Serial_485::set_baudrate (struct termios *opt, unsigned int baudrate) {
    cfsetispeed(opt, baudrate);
    cfsetospeed(opt, baudrate);
}
void Serial_485::set_data_bit (struct termios *opt, unsigned int databit) {
    opt->c_cflag &= ~CSIZE;
    switch (databit) {
    case 8:
        opt->c_cflag |= CS8;
        break;
    case 7:
        opt->c_cflag |= CS7;
        break;
    case 6:
        opt->c_cflag |= CS6;
        break;
    case 5:
        opt->c_cflag |= CS5;
        break;
    default:
        opt->c_cflag |= CS8;
break;
    }
}
void Serial_485::set_parity(struct termios *opt, std::string parity) {
    if (parity == "N" || parity == "n") {  // 无校验
        opt->c_cflag &= ~PARENB;
    }
    else if (parity == "E" || parity == "e") {  // 偶校验
        opt->c_cflag |= PARENB;
        opt->c_cflag &= ~PARODD;
    }
    else if (parity == "O" || parity == "o") {  // 奇校验
        opt->c_cflag |= PARENB;
        opt->c_cflag |= PARODD;
    }
    else {  // 其他选择为无校验
        opt->c_cflag &= ~PARENB;
    }
}

void Serial_485::set_stopbit (struct termios *opt, const char *stopbit) {
    if (0 == strcmp (stopbit, "1")) {
        opt->c_cflag &= ~CSTOPB;                            /* 1位停止位t         */
    }  else if (0 == strcmp (stopbit, "1.5")) {
        opt->c_cflag &= ~CSTOPB;                            /* 1.5位停止位    */
    }  else if (0 == strcmp (stopbit, "2")) {
        opt->c_cflag |= CSTOPB; 
    }  else {
        opt->c_cflag &= ~CSTOPB;                             /* 1 位停止位        */
    }
}
int  Serial_485::set_port_attr (int fd,int  baudrate, int  databit, const char *stopbit, std::string parity, int vtime, int vmin )
{
    struct termios opt;
    tcgetattr(fd, &opt);
    set_baudrate(&opt, baudrate);    
    set_data_bit(&opt, databit);
    set_parity(&opt, parity);
    set_stopbit(&opt, stopbit);
    opt.c_iflag &= ~(IXON | IXOFF | IXANY);
    opt.c_oflag &= ~OPOST;
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    opt.c_cc[VTIME]     = vtime;        
    opt.c_cc[VMIN]      = vmin; 
    tcflush (fd, TCIFLUSH);
    return (tcsetattr (fd, TCSANOW, &opt));
}
