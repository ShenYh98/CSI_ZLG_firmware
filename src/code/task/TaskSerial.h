#pragma once

#include "TaskAbstract.h"

#include "../protocol/Modbus.h"
#include "../serial/Serial_485.h"

using namespace ProtocolMiddleware;
using namespace SerialMiddleware;

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
    std::unordered_map< std::string, std::unique_ptr<SerialAbstract> > serialInfoMap;
    std::unordered_map< std::string, std::unique_ptr<Protocol> > protocolInfoMap;

    bool is_loadDevList;
    bool is_loadSerialList;
    bool is_mergeDevAndSerial;

private:
    void loadAllList();
};

}
