#ifndef TCPSOC
#define TCPSOC

#include "MyPacket.h"
#include <netinet/in.h>

class Socket{
private:
    int soc;
    int protocol;
    sockaddr_in listenAddr;

public:
    Socket();
    Socket(int socktype);
    Socket(const char server_ip[], const char port[], int socktype);

    int sendPacket(Packet *packet);
    std::string receiveData();
    bool dataAvailable();
    int rebindSocket();
};

#endif