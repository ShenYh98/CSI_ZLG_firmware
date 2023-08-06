#pragma once

#include <iostream>
#include <string>

class NetWorkLayer {
public:
    virtual void operation(int taskId) = 0;
private:
    virtual void receive(std::string& data) = 0;
    virtual void send(std::string& data) = 0;
};
