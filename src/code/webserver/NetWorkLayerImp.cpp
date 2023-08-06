#include "NetWorkLayerImp.h"

using namespace NetWorkMiddleware;

NetWorkLayerImp::NetWorkLayerImp(NetworkService* networkSrv) {
    _networkSrv = networkSrv;
}

NetWorkLayerImp::~NetWorkLayerImp() {

}

void NetWorkLayerImp::receive(std::string& data) {
    _networkSrv->receive(data);
}

void  NetWorkLayerImp::send(std::string& data) {
    _networkSrv->send(data);
}

void  NetWorkLayerImp::operation(int taskId) {
    switch (taskId)
    {
    case taskId::common:
        sendCommonInfo();
        break;
    
    default:
        break;
    }
}

void NetWorkLayerImp::sendCommonInfo() {
    double voltage = 220.0;
    double current = 10.0;
    double temperature = 25.0;

    std::string data = std::to_string(voltage) + "," + std::to_string(current) + "," + std::to_string(temperature);
    this->send(data);
    std::cout << data << std::endl;
}
