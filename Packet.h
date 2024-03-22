#ifndef PACKET
#define PACKET

#include <string>

enum PacketType{
    NONE,
    ERR,
    REPLY,
    AUTH,
    JOIN,
    MSG,
    BYE,
    CONFIRM
};

class Packet{
private:
public:
    virtual PacketType getType(){ return NONE; };
    virtual std::string getData(){ return ""; };
};

Packet* getPacket(std::string data);

#endif