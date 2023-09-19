#pragma once

#include "TaskAbstract.h"

#include "../webserver/HttpService.h"
#include "../webserver/NetWorkLayerImp.h"

using namespace NetWorkMiddleware;

// #define PATH        "192.168.1.136"
// #define PORT        9000
#define PATH        "172.16.138.129"
#define PORT        8081

namespace TaskMiddleWare {

class TaskHttp : public TaskAbstract {
public:
    TaskHttp();
    ~TaskHttp();

    virtual void start() override;
    virtual void pause() override;
    virtual void resume() override;
    virtual void stop() override;

private:
    NetworkService* ptr_httpService;
    NetWorkLayer* ptr_httpWork;

    std::thread http_thread;
    std::condition_variable handle_cv;
    std::mutex handle_mutex;
    bool isStopThread;
    taskInfo taskinfo;
};

}
