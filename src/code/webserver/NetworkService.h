#pragma once

#include <iostream>
#include <string>
#include <set>

class NetworkService {
public:
    virtual void receive(std::string& data) = 0;
    virtual void send(std::string& data) = 0;
};
