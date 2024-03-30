#include "UDPPackets.h"

//ERR packet
UDPPacketErr::UDPPacketErr(uint16_t messageId, std::string displayName, std::string messageContent)
{
    this->displayName = displayName;
    this->messageContent = messageContent;
    this->messageId = messageId;
}

PacketType UDPPacketErr::getType(){ return ERR; }

std::string UDPPacketErr::getData()
{
    //set data
    std::string output = Packet::getUDPHeader(this->getType(), this->messageId);
    output.append(this->displayName);
    output += '\x00';
    output.append(this->messageContent);
    output += '\x00';    

    //return data
    return output;
}

std::string UDPPacketErr::getMessage()
{
    return "ERR FROM " + this->displayName + ": " + this->messageContent + "\n";
}

uint16_t UDPPacketErr::getMessageId()
{
    return this->messageId;
}

//REPLY packet
UDPPacketReply::UDPPacketReply(uint16_t messageId, bool is_ok, uint16_t refMessageId, std::string messageContent)
{   
    this->is_ok = is_ok;
    this->messageContent = messageContent;
    this->messageId = messageId;
    this->refMessageId = refMessageId;
}

PacketType UDPPacketReply::getType(){ return REPLY; }

std::string UDPPacketReply::getData()
{
    //set data
    std::string output;
    output.append(Packet::getUDPHeader(this->getType(), this->messageId));
    output += (this->is_ok ? '\x01' : '\x00');
    output += this->refMessageId;
    output.append(this->messageContent);
    output += '\x00';

    //return data
    return output;
}

bool UDPPacketReply::getValidity()
{
    return this->is_ok;
}

std::string UDPPacketReply::getMessage()
{
    std::string ok = this->is_ok?"Success: ":"Failure: ";
    return ok + this->messageContent + "\n";
}

uint16_t UDPPacketReply::getMessageId()
{
    return this->messageId;
}

//AUTH packet
UDPPacketAuth::UDPPacketAuth(uint16_t messageId, std::string username, std::string secret, std::string displayName)
{
    this->username = username;
    this->displayName = displayName;
    this->secret = secret;
    this->messageId = messageId;
}

PacketType UDPPacketAuth::getType()
{
    return AUTH;
}

std::string UDPPacketAuth::getData()
{
    //set data
    std::string output;
    output.append(Packet::getUDPHeader(this->getType(), this->messageId));
    output.append(this->username);
    output += '\x00';
    output.append(this->displayName);
    output += '\x00';
    output.append(this->secret);
    output += '\x00';

    //return data
    return output;
}

uint16_t UDPPacketAuth::getMessageId()
{
    return this->messageId;
}

//JOIN packet
UDPPacketJoin::UDPPacketJoin(uint16_t messageId, std::string chanelID, std::string displayName)
{
    this->chanelID = chanelID;
    this->displayName = displayName;
    this->messageId = messageId;
}

PacketType UDPPacketJoin::getType(){ return JOIN; }

std::string UDPPacketJoin::getData()
{
    //set data
    std::string output = Packet::getUDPHeader(this->getType(), this->messageId);
    output.append(this->chanelID);
    output += '\x00';
    output.append(this->displayName);
    output += '\x00';


    //return data
    return output;
}

uint16_t UDPPacketJoin::getMessageId()
{
    return this->messageId;
}

//MSG packet
UDPPacketMsg::UDPPacketMsg(uint16_t messageId, std::string displayName, std::string messageContent)
{   
    this->displayName = displayName;
    this->messageContent = messageContent;
    this->messageId = messageId;
}

PacketType UDPPacketMsg::getType(){ return MSG; }

std::string UDPPacketMsg::getData()
{
    //set data
    std::string output = Packet::getUDPHeader(this->getType(), this->messageId);
    output.append(this->displayName);
    output += '\x00';
    output.append(this->messageContent);
    output += '\x00';

    //return data
    return output;
}

std::string UDPPacketMsg::getMessage()
{
    return this->displayName + ": " + this->messageContent + "\n";
}

uint16_t UDPPacketMsg::getMessageId()
{
    return this->messageId;
}

//BYE packet
UDPPacketBye::UDPPacketBye(uint16_t messageId)
{
    this->messageId = messageId;
}

PacketType UDPPacketBye::getType(){ return BYE; }

std::string UDPPacketBye::getData()
{
    //set data
    std::string output = Packet::getUDPHeader(this->getType(), this->messageId);
    //return data
    return output;
}

uint16_t UDPPacketBye::getMessageId()
{
    return this->messageId;
}

//CONFIRM packet
UDPPacketConfirm::UDPPacketConfirm(uint16_t messageId)
{
    this->messageId = messageId;
}

PacketType UDPPacketConfirm::getType(){ return CONFIRM; }

std::string UDPPacketConfirm::getData()
{
    //set data
    std::string output = Packet::getUDPHeader(this->getType(), this->messageId);
    //return data
    return output;
}

uint16_t UDPPacketConfirm::getMessageId()
{
    return this->messageId;
}