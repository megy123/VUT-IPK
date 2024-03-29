#ifndef TCPSOC
#define TCPSOC

#include "MyPacket.h"

class Socket{
private:
    int soc;
    int protocol;

public:
    Socket();
    Socket(const char server_ip[], const char port[], int socktype);

    int sendPacket(Packet *packet);
    std::string receiveData();
    bool dataAvailable();
    void rebindSocket(int port);
};

#endif