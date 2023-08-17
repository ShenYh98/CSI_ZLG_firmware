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
    GetChannelTable
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
} UartParamInfo;

typedef struct {
    int UartId;             // 创建的id,通过打开文件来获取
    int statu;              // 0未使能 1使能
    std::string UartName;   // 驱动文件名
    UartParamInfo uartParamInfo;
} UartIdInfo;

