#pragma once

#include <iostream>

#include "../common/CommonLog.h"
#include "../common/MessageQueue.hpp"
#include "../../DataStructure.h"

using namespace CommonLib;

class TaskAbstract {
public:
    virtual void start() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void stop() = 0;
};
