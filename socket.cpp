#include "socket.h"
#include "MyPacket.h"
#include "TCPPackets.h"
#include "UDPPackets.h"
#include "parser.h"
#include <iostream>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
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
    this->protocol = socktype;
    //resolve addrinfo
    struct addrinfo *server_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = socktype;    // TCP/UDP
    hints.ai_protocol = 0;           // Protocol

    int status = getaddrinfo(server_ip, port, &hints, &server_info);

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


    sockaddr_in sendAddr;
	sendAddr.sin_family = AF_INET;
    struct hostent *he = gethostbyname(server_ip);
    memcpy(&sendAddr.sin_addr, he->h_addr_list[0], he->h_length);
    //sendAddr.sin_addr.s_addr = (server_info->ai_addr)->sin_addr;//inet_addr(server_ip);
	sendAddr.sin_port = htons(std::stoi(port));

    this->listenAddr = sendAddr;

    //binding
    sockaddr_in recvAddress;
	recvAddress.sin_family = AF_INET;
	recvAddress.sin_addr.s_addr = INADDR_ANY;
	recvAddress.sin_port = 0;

	if (bind(this->soc, (const sockaddr*)&recvAddress, sizeof(sockaddr_in)) < 0)
	{
		std::cerr << "Failed to bind socket." << std::endl;
		return;
	}

    //connect
    if(socktype == SOCK_STREAM)
    {
        if(connect(this->soc, server_info->ai_addr, server_info->ai_addrlen) < 0)
        {
            std::cerr << "Failed to connect.\n";
            exit(1);        
        }
    }

}

int Socket::sendPacket(Packet *packet, int retransmissions, int timeout)
{
    //UDP variant
    //get data
    std::string data;
    uint16_t messageId = 0;
    switch (packet->getType())
    {
    case ERR:
        data = dynamic_cast<UDPPacketErr *>(packet)->getData();
        messageId = dynamic_cast<UDPPacketErr *>(packet)->getMessageId();
        break;
    case REPLY:
        data = dynamic_cast<UDPPacketReply *>(packet)->getData();
        messageId = dynamic_cast<UDPPacketReply *>(packet)->getMessageId();
        break;
    case AUTH:
        data = dynamic_cast<UDPPacketAuth *>(packet)->getData();
        messageId = dynamic_cast<UDPPacketAuth *>(packet)->getMessageId();
        break;
    case JOIN:
        data = dynamic_cast<UDPPacketJoin *>(packet)->getData();
        messageId = dynamic_cast<UDPPacketJoin *>(packet)->getMessageId();
        break;
    case MSG:
        data = dynamic_cast<UDPPacketMsg *>(packet)->getData();
        messageId = dynamic_cast<UDPPacketMsg *>(packet)->getMessageId();
        break;
    case BYE:
        data = dynamic_cast<UDPPacketBye *>(packet)->getData();
        messageId = dynamic_cast<UDPPacketBye *>(packet)->getMessageId();
        break;
    case CONFIRM:
        data = dynamic_cast<UDPPacketConfirm *>(packet)->getData();
        messageId = dynamic_cast<UDPPacketConfirm *>(packet)->getMessageId();
        break;
    default:
        std::cerr << "Invalid packet!\n";
        exit(1);
        break;
    }

    fd_set fds = { 0 }; // will be checked for being ready to read
    FD_ZERO(&fds);
    FD_SET(this->soc, &fds);

    // sets timeout
    struct timeval tv = { 0 };
    tv.tv_sec = 0;
    tv.tv_usec = timeout * 1000; //microseconds


    while(retransmissions > 0){
        //send data
        sendto(this->soc, data.c_str(), data.size(), 0,(struct sockaddr*)&this->listenAddr, sizeof(this->listenAddr));
        if(packet->getType() == CONFIRM)return 0;

        //await confirm packet
        int ret = select( this->soc + 1, &fds, NULL, NULL, &tv);
        if (  ret < 0 )
        {
            // error on select()
            std::cerr << "ERR: Error in select.\n";
        }
        else if ( ret == 0 )
        {
            // timeout
            std::cerr << "PICA\n";
            retransmissions--;
        }
        else if ( FD_ISSET( this->soc, &fds ) && dataAvailable() ) // data to read?
        {   
            //expect confirm
            std::string data = receiveData();

            Packet* confirm = resolveUDPPacket(data);
            if(confirm->getType() != CONFIRM)return 1;
            if(dynamic_cast<UDPPacketConfirm *>(confirm)->getMessageId() > messageId)return 1;
            return 0;
        }
    }
    return 0;
}

int Socket::sendPacket(Packet *packet)
{
    //TCP variant
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
        return 1;
        break;
    }

    //send data
    send(this->soc, data.c_str(), strlen(data.c_str()), 0);

    return 0;
}

std::string Socket::receiveData()
{
    //receive data
    std::string output;
    char buffer[1024] = {0};
    socklen_t cliAddrLen = sizeof(this->listenAddr);
    int num = recvfrom(this->soc, buffer, 1024, 0, (struct sockaddr*)&this->listenAddr, &cliAddrLen);
    for(int i=0;i<num;i++)
    {
        output += buffer[i];
    }

    return output;
}

bool Socket::dataAvailable()
{
    int count;
    ioctl(this->soc, FIONREAD, &count);
    return count;
}