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
    Socket(const char server_ip[], const char port[], int socktype);

    int sendPacket(Packet *packet, int retransmissions, int timeout);//UDP version
    int sendPacket(Packet *packet);//TCP version
    std::string receiveData();
    bool dataAvailable();
};

#endif