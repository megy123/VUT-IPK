#ifndef TCPSOC
#define TCPSOC

#include "MyPacket.h"

class TCPSocket{
private:
    int soc;

public:
    TCPSocket();
    TCPSocket(const char server_ip[], int port);

    void sendPacket(Packet *packet);
    std::string receiveData();
    bool dataAvailable();
};

#endif