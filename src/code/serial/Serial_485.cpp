#include "Serial_485.h"

using namespace SerialMiddleware;

Serial_485::Serial_485(const std::string& jsonData) {
    if ( json_get_param(jsonData) ) {
        serialIdInfo.SerialId = openDriver(serialIdInfo.SerialName);
        serialIdInfo.statu = 0;
        set_port_attr(serialIdInfo.SerialId, 
                      serialIdInfo.serialParamInfo.baudrate,
                      serialIdInfo.serialParamInfo.databit,
                      serialIdInfo.serialParamInfo.stopbit.c_str(),
                      serialIdInfo.serialParamInfo.parity,
                      serialIdInfo.serialParamInfo.vtime,
                      serialIdInfo.serialParamInfo.vmin);
    }
}

Serial_485::~Serial_485() {
}

void Serial_485::receive(char* buf) {
    int len;
    len = read(serialIdInfo.SerialId, buf, sizeof(buf));                    /* 在串口读入字符串 */
    if (len < 0) {
        LOG_ERROR("read error \n");
    }
    LOG_DEBUG("recv buf: {}\n", buf);
}
void Serial_485::send(const char* buf) {
    int len;
    len = write(serialIdInfo.SerialId, buf, sizeof(buf));  // 串口写入字符串
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

int Serial_485::json_get_param(const std::string& jsonData) {
    // 来自http的JSON数据
    /*---------------------
    {
        "Serialname" : "/dev/ttyRS485-1",
        "param" : {
            "baudrate" : B9600,
            "databit" : 8,
            "stopbit" : "1",
            "parity" : "N",
            "vtime" : 150,
            "vmin" : 255
        }
    }
    ----------------------*/
    json http_jsonData;
    try {
        http_jsonData = json::parse(jsonData);

        // 验证JSON数据的有效性
        if (!http_jsonData.is_object()) {
            throw std::runtime_error("Invalid JSON data: not an object");
        }

        // 获取字段的值
        serialIdInfo.SerialName = http_jsonData["Serialname"].get<std::string>();
        serialIdInfo.serialParamInfo.baudrate = http_jsonData["param"]["baudrate"].get<int>();
        serialIdInfo.serialParamInfo.databit = http_jsonData["param"]["databit"].get<int>();
        serialIdInfo.serialParamInfo.parity = http_jsonData["param"]["stopbit"].get<int>();
        serialIdInfo.serialParamInfo.stopbit = http_jsonData["param"]["parity"].get<std::string>();
        serialIdInfo.serialParamInfo.vmin = http_jsonData["param"]["vtime"].get<int>();
        serialIdInfo.serialParamInfo.vtime = http_jsonData["param"]["vmin"].get<int>();
    } catch(const std::exception& e) {
        // Json解析错误
        LOG_ERROR("Json解析错误\n");
        return 0;
    }
    return 1;
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
void Serial_485::set_parity (struct termios *opt, char parity) {
    switch (parity) {
    case 'N':                                              /* 无校验        */
case 'n':
        opt->c_cflag &= ~PARENB;
        break;
     case 'E':                                              /* 偶校验        */
case 'e':
        opt->c_cflag |= PARENB;
    opt->c_cflag &= ~PARODD;
    break;
    case 'O':                                              /* 奇校验            */
case 'o':
        opt->c_cflag |= PARENB;
    opt->c_cflag |= ~PARODD;
    break;
    default:                                                 /* 其它选择为无校验 */
    opt->c_cflag &= ~PARENB;
    break;
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
int  Serial_485::set_port_attr (int fd,int  baudrate, int  databit, const char *stopbit, char parity, int vtime,int vmin )
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
