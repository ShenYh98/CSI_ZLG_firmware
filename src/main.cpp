#include <iostream>
#include <fstream>
#include <cstring>

#include <mutex>
#include <functional>
#include <condition_variable>

#include "code/common/CommonLog.h"
#include "code/common/ThreadPool.h"
#include "code/common/MessageQueue.hpp"

#include "inc_plug/Setting.h"

#include "code/webserver/NetWorkLayerImp.h"

#include "code/task/TaskHttp.h"
#include "code/task/TaskWebSocket.h"
#include "code/task/TaskSerial.h"

#include "DataStructure.h"

using namespace CommonLib;
using namespace NetWorkMiddleware;
using namespace SerialMiddleware;
using namespace SettingMiddleware;
using namespace ProtocolMiddleware;
using namespace TaskMiddleWare;

int main() {
    COMMONLOG_INIT("config/csi_config.json");

    std::unique_ptr<TaskAbstract> task_http = std::make_unique<TaskHttp>();
    std::unique_ptr<TaskAbstract> task_websocket = std::make_unique<TaskWebSocket>();
    std::unique_ptr<TaskAbstract> task_serial = std::make_unique<TaskSerial>();
    std::unique_ptr<Setting> setting = std::make_unique<Setting>();

    task_http->start();
    task_websocket->start();
    task_serial->start();

    while (true) {
    }

    return 0;
}