#pragma once

#include <iostream>
#include <string>
#include <set>

#include <mutex>
#include <semaphore.h>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

static sem_t sem;
static std::mutex cv_m;
static bool response_ready = false;

class NetworkService {
public:
    virtual void receive(std::string& data) = 0;
    virtual void send(std::string& data) = 0;
};
