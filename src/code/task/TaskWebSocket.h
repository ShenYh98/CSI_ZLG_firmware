#pragma once

#include "TaskAbstract.h"

#include "../webserver/WebSocketService.h"
#include "../webserver/NetWorkLayerImp.h"

using namespace NetWorkMiddleware;

namespace TaskMiddleWare {

class TaskWebSocket : public TaskAbstract {
public:
    TaskWebSocket();
    ~TaskWebSocket();

    virtual void start() override;
    virtual void pause() override;
    virtual void resume() override;
    virtual void stop() override;

private:
    NetworkService* ptr_wsService;
    NetWorkLayer* ptr_wsWork;

    std::thread ws_thread;
    std::condition_variable handle_cv;
    std::mutex handle_mutex;
    bool isStopThread;
    taskInfo taskinfo;

    s_RTtask rttask;
};

}
