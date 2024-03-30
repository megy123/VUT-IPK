#include "MyPacket.h"



std::string Packet::getUDPHeader(PacketType type, uint16_t messageID)
{
    //init helper variables
    std::string output;
    unsigned char buf[2];
    buf[0] = messageID;
    buf[1] = messageID >> 8;

    //set up header
    output += UDPMessageType[type];
    output += buf[1];
    output += buf[0];

    return output;
}