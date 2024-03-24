#ifndef PACKET
#define PACKET

#include <string>
#include <map>

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
    std::map<PacketType, char> UDPMessageType {
    {CONFIRM, '\x00'},
    {REPLY, '\x01'},
    {AUTH, '\x02'},
    {JOIN, '\x03'},
    {MSG, '\x04'},
    {ERR, '\xFE'},
    {BYE, '\xFF'}
    };

    std::string getUDPHeader(PacketType type, uint16_t messageID);
    virtual PacketType getType(){ return NONE; };
    virtual std::string getData(){ return ""; };
};


#endif