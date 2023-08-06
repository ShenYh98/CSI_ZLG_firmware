#pragma once

#include "NetWorkLayer.h"
#include "NetworkService.h"

namespace NetWorkMiddleware {
    
typedef enum {
    common
} taskId;

class NetWorkLayerImp : public NetWorkLayer {
public:
    // 以何种网络协议通信
    NetWorkLayerImp(NetworkService* networkSrv);
    ~NetWorkLayerImp();

    void operation(int taskId) override;
private:
    void receive(std::string& data) override;
    void send(std::string& data) override;

    void sendCommonInfo();

private:
    NetworkService* _networkSrv;
};

}


