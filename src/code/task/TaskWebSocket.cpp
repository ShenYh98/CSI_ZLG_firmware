#include "TaskWebSocket.h"

using namespace TaskMiddleWare;

TaskWebSocket::TaskWebSocket() {
    ptr_wsService = new WebSocketService();
    ptr_wsWork = new NetWorkLayerImp(ptr_wsService);

    taskinfo.taskname = "WebSocket";
    taskinfo.taskstate = taskState::ready;
    isStopThread = false;

    rttask.isOn = 0;
    rttask.dev_sn = "";

    // 订阅实时监控任务
    MessageQueue<s_RTtask>::getInstance().subscribe("TaskWebSocket/getRtData", [&](const s_RTtask& task) {
        LOG_DEBUG("Received id: {}\n", task.id);
        LOG_DEBUG("Received dev_sn: {}\n", task.dev_sn);
        LOG_DEBUG("Received isOn: {}\n", task.isOn);

        rttask = task;
    });

    ws_thread = std::thread([&]() {
        LOG_DEBUG("[TaskWebSocket]websocket thread is run\n");

        while (true) {
            while (taskinfo.taskstate != taskState::start) {
                std::unique_lock<std::mutex> lock(handle_mutex);

                LOG_DEBUG("[TaskWebSocket]work wait\n");
                if (!isStopThread) {
                    handle_cv.wait(lock);
                } 
                if (isStopThread) {
                    return;
                }
                LOG_DEBUG("[TaskWebSocket]work start\n");

                taskinfo.taskstate = taskState::start;
            }
            
            if (rttask.isOn) { // 需要http下发启动指令，再去遥测
                ptr_wsWork->operation(rttask);
            }
            std::this_thread::sleep_for(3s);
        }
    });
}

TaskWebSocket::~TaskWebSocket() {

}

void TaskWebSocket::start() {
    LOG_DEBUG("[TaskWebSocket]work start, previous state: %d\n", taskinfo.taskstate);

    std::unique_lock<std::mutex> lock(handle_mutex);

    taskinfo.taskstate = taskState::start;
    handle_cv.notify_all();
}

void TaskWebSocket::pause() {
    LOG_DEBUG("[TaskWebSocket]work pause, previous state: %d\n", taskinfo.taskstate);
    
    std::unique_lock<std::mutex> lock(handle_mutex);

    taskinfo.taskstate = taskState::stop;
}

void TaskWebSocket::resume() {
    LOG_DEBUG("[TaskWebSocket]work resume, previous state: %d\n", taskinfo.taskstate);
    
    std::unique_lock<std::mutex> lock(handle_mutex);

    if (taskinfo.taskstate != taskState::cancel) {
        taskinfo.taskstate = taskState::start;
    }
    handle_cv.notify_all();
}

void TaskWebSocket::stop() {
    LOG_DEBUG("[TaskWebSocket]work stop, previous state: %d\n", taskinfo.taskstate);

    std::unique_lock<std::mutex> lock(handle_mutex);

    taskinfo.taskstate = taskState::cancel;
}