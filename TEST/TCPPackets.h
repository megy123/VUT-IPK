/*
Project:    IPK 1. projekt
File:       TCPPackets.h
Authors:    Dominik Sajko (xsajko01)
Date:       31.03.2024
*/
#ifndef TCP_PACKETS
#define TCP_PACKETS

#include <string>
#include "MyPacket.h"

//ERR packet
class TCPPacketErr : public Packet{
private:
    std::string displayName;
    std::string messageContent;

public:
    PacketType getType();
    TCPPacketErr(std::string displayName, std::string messageContent);

    std::string getData();
    std::string getMessage();
};

//REPLY packet
class TCPPacketReply : public Packet{
private:
    bool is_ok;
    std::string messageContent;

public:
    PacketType getType();
    TCPPacketReply(bool is_ok, std::string messageContent);

    std::string getData();
    bool getValidity();
    std::string getMessage();
};

//AUTH packet
class TCPPacketAuth : public Packet{
private:
    std::string username;
    std::string displayName;
    std::string secret;

public:
    PacketType getType();
    TCPPacketAuth(std::string username, std::string displayName, std::string secret);

    std::string getData();
};

//JOIN packet
class TCPPacketJoin : public Packet{
private:
    std::string chanelID;
    std::string displayName;


public:
    PacketType getType();
    TCPPacketJoin(std::string chanelID, std::string displayName);

    std::string getData();
};

//MESSAGE packet
class TCPPacketMsg : public Packet{
private:
    std::string displayName;
    std::string messageContent;

public:
    PacketType getType();
    TCPPacketMsg(std::string displayName, std::string messageContent);

    std::string getData();
    std::string getMessage();
    
};

//BYE packet
class TCPPacketBye : public Packet{
private:
public:
    PacketType getType();
    std::string getData();
};


#endif