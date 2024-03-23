#include "TCPsocket.h"
#include <iostream>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "MyPacket.h"
#include "TCPPackets.h"

TCPSocket::TCPSocket(){}

TCPSocket::TCPSocket(const char server_ip[], int port){

    struct sockaddr_in serv_addr;

    if ((this->soc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Error while creating socket!\n";
        throw 1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, server_ip, &serv_addr.sin_addr)<=0)
    {
        std::cerr << "Invalid address!\n";
        throw 1;
    }

    if (connect(this->soc, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Failed to connect to server!\n";
        throw 1;
    }
}

void TCPSocket::sendPacket(Packet *packet)
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
    //printf("--message sent\n");


}

std::string TCPSocket::receiveData()
{
    //receive data
    char buffer[1024] = {0};
    read(this->soc, buffer, 1024);
    //printf("--message received\n");
    //std::cout << buffer;

    return buffer;
}

bool TCPSocket::dataAvailable()
{
    int count;
    ioctl(this->soc, FIONREAD, &count);
    return count;
}