#ifndef UDP_PACKETS
#define UDP_PACKETS

#include <string>
#include "MyPacket.h"


//ERR packet
class UDPPacketErr : public Packet{
private:
    std::string displayName;
    std::string messageContent;
    uint16_t messageId;

public:
    PacketType getType();
    UDPPacketErr(uint16_t messageId, std::string displayName, std::string messageContent);

    std::string getData();
    std::string getMessage();
    uint16_t getMessageId();
};

//REPLY packet
class UDPPacketReply : public Packet{
private:
    bool is_ok;
    std::string messageContent;
    uint16_t messageId;
    uint16_t refMessageId;

public:
    PacketType getType();
    UDPPacketReply(uint16_t messageId, bool is_ok, uint16_t refMessageId, std::string messageContent);

    std::string getData();
    bool getValidity();
    std::string getMessage();
    uint16_t getMessageId();
};

//AUTH packet
class UDPPacketAuth : public Packet{
private:
    std::string username;
    std::string displayName;
    std::string secret;
    uint16_t messageId;

public:
    PacketType getType();
    UDPPacketAuth(uint16_t messageId, std::string username, std::string secret, std::string displayName);

    std::string getData();
    uint16_t getMessageId();
};

//JOIN packet
class UDPPacketJoin : public Packet{
private:
    std::string chanelID;
    std::string displayName;
    uint16_t messageId;


public:
    PacketType getType();
    UDPPacketJoin(uint16_t messageId, std::string chanelID, std::string displayName);

    std::string getData();
    uint16_t getMessageId();
};

//MESSAGE packet
class UDPPacketMsg : public Packet{
private:
    std::string displayName;
    std::string messageContent;
    uint16_t messageId;

public:
    PacketType getType();
    UDPPacketMsg(uint16_t messageId, std::string displayName, std::string messageContent);

    std::string getData();
    std::string getMessage();
    uint16_t getMessageId();
};

//BYE packet
class UDPPacketBye : public Packet{
private:
    uint16_t messageId;

public:
    UDPPacketBye(uint16_t messageId);

    PacketType getType();
    std::string getData();
    uint16_t getMessageId();
};

//CONFIRM packet
class UDPPacketConfirm : public Packet{
private:
    uint16_t messageId;
public:
    UDPPacketConfirm(uint16_t messageId);

    PacketType getType();
    std::string getData();
    uint16_t getMessageId();
};

#endif