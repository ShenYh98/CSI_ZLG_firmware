#pragma once

#include "TaskAbstract.h"

#include "../protocol/Modbus.h"
#include "../serial/Serial_485.h"

using namespace ProtocolMiddleware;
using namespace SerialMiddleware;

#define RECVBUF     512
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

    std::shared_ptr<Protocol> pit;
    std::thread recv_thread;
    std::condition_variable recv_cv;
    std::mutex recv_mutex;
    std::mutex recv_wait_mutex;

    std::vector<uint8_t> total_recv_buf;

    bool is_RecvRun;
    int pointCount;

    std::string pscProtocolPath;
    std::vector<modbusPoint> v_points;
    std::vector<s_RTSrcInfo> v_singleRTSrcInfo;
private:
    void loadAllList();
    void serialTaskRun();
    
    bool sendPacketProc(s_RTSrcInfo& rtSrcInfo);
    bool parsePacketProc(s_RTSrcInfo& rtSrcInfo);

    void handleAllRecvData(vector<s_RTSrcInfo>& v_rtSrcInfo);
    void handleRecvData(s_RTSrcInfo& it, const int&& haddr, const int&& taddr);
    int loadPcsProtocolByJson(const std::string filename);
};

}
