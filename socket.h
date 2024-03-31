/*
Project:    IPK 1. projekt
File:       socket.h
Authors:    Dominik Sajko (xsajko01)
Date:       31.03.2024
*/
#ifndef TCPSOC
#define TCPSOC

#include "MyPacket.h"
#include <netinet/in.h>

class Socket{
private:
    int soc;
    int protocol;
    sockaddr_in senderAddress;

public:
    Socket();
    Socket(const char server_ip[], const char port[], int socktype);

    int sendPacket(Packet *packet, int retransmissions, int timeout);//UDP version
    int sendPacket(Packet *packet);//TCP version
    std::string receiveData();
    bool dataAvailable();
};

#endif