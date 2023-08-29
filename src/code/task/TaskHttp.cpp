#include "TaskHttp.h"

using namespace TaskMiddleWare;

TaskHttp::TaskHttp () {
    ptr_httpService = new HttpService(PATH, PORT);
    ptr_httpWork = new NetWorkLayerImp(ptr_httpService);

    taskinfo.taskname = "Http";
    taskinfo.taskstate = taskState::ready;
    isStopThread = false;

    http_thread = std::thread([&]() {
        LOG_DEBUG("[TaskHttp]http thread is run\n");

        while (true) {
            while (taskinfo.taskstate != taskState::start) {
                std::unique_lock<std::mutex> lock(handle_mutex);

                LOG_DEBUG("[TaskHttp]work wait\n");
                if (!isStopThread) {
                    handle_cv.wait(lock);
                } 
                if (isStopThread) {
                    return;
                }
                LOG_DEBUG("[TaskHttp]work start\n");

                taskinfo.taskstate = taskState::start;
            }
            
            ptr_httpWork->operation();
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    });
}

TaskHttp::~TaskHttp () {
    {
        std::unique_lock<std::mutex> lock(handle_mutex);
        isStopThread = true;
        taskinfo.taskstate = taskState::cancel;
        handle_cv.notify_all(); // 通知所有等待的线程
    }
    http_thread.join();

    delete(ptr_httpWork);
    ptr_httpWork = nullptr; // 防止悬挂指针
    delete(ptr_httpService);
    ptr_httpService = nullptr;
}

void TaskHttp::start() {
    LOG_DEBUG("[TaskHttp]work start, previous state: %d\n", taskinfo.taskstate);

    std::unique_lock<std::mutex> lock(handle_mutex);

    taskinfo.taskstate = taskState::start;
    handle_cv.notify_all();
}

void TaskHttp::pause() {
    LOG_DEBUG("[TaskHttp]work pause, previous state: %d\n", taskinfo.taskstate);
    
    std::unique_lock<std::mutex> lock(handle_mutex);

    taskinfo.taskstate = taskState::stop;
}

void TaskHttp::resume() {
    LOG_DEBUG("[TaskHttp]work resume, previous state: %d\n", taskinfo.taskstate);
    
    std::unique_lock<std::mutex> lock(handle_mutex);

    if (taskinfo.taskstate != taskState::cancel) {
        taskinfo.taskstate = taskState::start;
    }
    handle_cv.notify_all();
}

void TaskHttp::stop() {
    LOG_DEBUG("[TaskHttp]work stop, previous state: %d\n", taskinfo.taskstate);

    std::unique_lock<std::mutex> lock(handle_mutex);

    taskinfo.taskstate = taskState::cancel;
}
