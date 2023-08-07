#pragma once

#include "NetWorkLayer.h"
#include "NetworkService.h"

#include "nlohmann/json.hpp"

namespace NetWorkMiddleware {
    
typedef enum {
    GetPassWord
} taskId;

class NetWorkLayerImp : public NetWorkLayer {
public:
    // 以何种网络协议通信
    NetWorkLayerImp(NetworkService* networkSrv);
    ~NetWorkLayerImp();

    void operation() override;
private:
    void receive(std::string& data) override;
    void send(std::string& data) override;

    int getPassWord(const std::string filename, const std::string recv_data);
    int getTaskId(const std::string recv_data);

private:
    NetworkService* _networkSrv;
};

}


