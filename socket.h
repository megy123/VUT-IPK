#ifndef TCPSOC
#define TCPSOC

#include "MyPacket.h"

class Socket{
private:
    int soc;

public:
    Socket();
    Socket(const char server_ip[], const char port[], int socktype);

    void sendPacket(Packet *packet);
    std::string receiveData();
    bool dataAvailable();
};

#endif