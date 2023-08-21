#pragma once

#include <iostream>

/*
 *   webserver通讯层的数据结构
 */
typedef enum {
    GetPassWord,
    DevTableSave,
    ChannelTableSave,
    GetDevTable,
    GetChannelTable,
    AddDev,
    EditDev,
    DelDev
} taskId;

typedef enum {
    Yc,         // 遥测
    Yx,         // 遥信
    Yk,         // 遥控
    Yt          // 遥调
} RTtaskId;

typedef struct {
    int isOn;
    std::string dev_sn;
    RTtaskId id;
} s_RTtask;

/*
 *   串口的数据结构
 */

typedef struct {
    int baudrate;           // 波特率
    int databit;            // 数据为
    std::string stopbit;    // 停止位
    char parity;            // 奇偶性
    int vtime;              // 最小字节
    int vmin;               // 最迟响应时间
} SerialParamInfo;

typedef struct {
    int SerialId;               // 创建的id,通过打开文件来获取
    int statu;                  // 0未使能 1使能
    std::string SerialName;     // 驱动文件名
    std::string name;           // 串口名
    SerialParamInfo serialParamInfo;
} SerialIdInfo;

typedef struct {
    std::string devName;        // 设备名
    SerialIdInfo serialInfo;    // 串口
    std::string protocol;       // 规约
    int addr;                   // 地址
    std::string category;       // 类型
    std::string model;          // 型号
    std::string sn;             // sn号
    int deviceStatus;           // 设备状态
} DevInfo;

/*
 *   Setting数据结构
 */

typedef enum {
    Add,
    Edit,
    Del
} Action;

typedef struct {
    Action act;
    DevInfo devInfo;
    DevInfo oldDevInfo;
} srv_DevInfo;
