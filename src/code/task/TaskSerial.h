#pragma once

#include "TaskAbstract.h"

#include "../protocol/Modbus.h"
#include "../serial/Serial_485.h"

using namespace ProtocolMiddleware;
using namespace SerialMiddleware;

#define RECVBUF     64
#define SENDBUF     8

namespace TaskMiddleWare {

class TaskSerial : public TaskAbstract {
public:
    TaskSerial();
    ~TaskSerial();

    virtual void start() override;
    virtual void pause() override;
    virtual void resume() override;
    virtual void stop() override;

private:
    std::thread serial_thread;
    std::condition_variable handle_cv;
    std::mutex handle_mutex; // 在任务开始暂停结束时，确保不会同时控制线程
    std::mutex list_mutex; // 在列表增删改查的时候，确保不会并发修改
    bool isStopThread;
    taskInfo taskinfo;

    std::vector<DevInfo> devList;
    std::vector<SerialIdInfo> serialList;
    std::unordered_map< std::string, std::shared_ptr<SerialAbstract> > serialInfoMap;
    std::unordered_map< std::string, std::shared_ptr<Protocol> > protocolInfoMap;

    bool is_loadDevList;
    bool is_loadSerialList;
    bool is_mergeDevAndSerial;

    std::vector<modbusPoint> v_point;

    std::thread recv_thread;
    std::shared_ptr<Protocol> pit;
    std::condition_variable recv_cv;
    std::mutex recv_mutex;
    std::mutex recv_wait_mutex;

    std::vector<uint8_t> total_recv_buf;

    bool is_RecvRun;
private:
    void loadAllList();
    int HexToDec(const uint8_t* data, std::size_t length);
};

}
