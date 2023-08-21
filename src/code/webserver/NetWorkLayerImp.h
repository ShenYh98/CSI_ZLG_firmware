#pragma once

#include "NetWorkLayer.h"
#include "NetworkService.h"

namespace NetWorkMiddleware {

class NetWorkLayerImp : public NetWorkLayer {
public:
    // 以何种网络协议通信
    NetWorkLayerImp(NetworkService* networkSrv);
    ~NetWorkLayerImp();

    void operation() override;
    void operation(RTtaskId rttaskId) override;
private:
    void receive(std::string& data) override;
    void send(std::string& data) override;

    int getTaskId(const std::string recv_data);
    int getPassWord(const std::string filename, const std::string recv_data);
    int devTableSave(const std::string filename, const std::string recv_data);
    int channelTableSave(const std::string filename, const std::string recv_data);
    int tableLoad(const std::string filename, std::string& data);

    int addDev(const std::string recv_data);
    int editDev(const std::string recv_data);
    int delDev(const std::string recv_data);

private:
    NetworkService* _networkSrv;
};

}


