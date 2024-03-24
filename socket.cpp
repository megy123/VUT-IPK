#include "socket.h"
#include <iostream>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "MyPacket.h"
#include "TCPPackets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <limits.h>
#include <stdbool.h>

Socket::Socket(){}

Socket::Socket(const char server_ip[], const char port[], int socktype){
    //resolve addrinfo
    struct addrinfo *server_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = socktype;    // TCP/UDP
    hints.ai_protocol = 0;           // Protocol

    int status = getaddrinfo(server_ip, "4567", &hints, &server_info);

    if (status != 0 || server_info->ai_addr == NULL)
    {
        std::cerr << "Failed to resolve hostname.\n";
        exit(1);
    }

    //init socket
    if((this->soc = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol)) < 0)
    {
        std::cerr << "Invalid socket.\n";
        exit(1);
    }

    //connect
    if(connect(this->soc, server_info->ai_addr, server_info->ai_addrlen) < 0)
    {
        std::cerr << "Failed to connect.\n";
        exit(1);        
    }
}

void Socket::sendPacket(Packet *packet)
{
    //get data
    std::string data;
    switch (packet->getType())
    {
    case ERR:
        data = dynamic_cast<TCPPacketErr *>(packet)->getData();
        break;
    case REPLY:
        data = dynamic_cast<TCPPacketReply *>(packet)->getData();
        break;
    case AUTH:
        data = dynamic_cast<TCPPacketAuth *>(packet)->getData();
        break;
    case JOIN:
        data = dynamic_cast<TCPPacketJoin *>(packet)->getData();
        break;
    case MSG:
        data = dynamic_cast<TCPPacketMsg *>(packet)->getData();
        break;
    case BYE:
        data = dynamic_cast<TCPPacketBye *>(packet)->getData();
        break;
    default:
        std::cerr << "Invalid packet!\n";
        throw 1;
        break;
    }

    //send data
    send(this->soc, data.c_str(), strlen(data.c_str()), 0);
}

std::string Socket::receiveData()
{
    //receive data
    char buffer[1024] = {0};
    read(this->soc, buffer, 1024);
    //printf("--message received\n");
    //std::cout << buffer;

    return buffer;
}

bool Socket::dataAvailable()
{
    int count;
    ioctl(this->soc, FIONREAD, &count);
    return count;
}