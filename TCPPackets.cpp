#include "TCPPackets.h"

// ERR	ERR FROM {DisplayName} IS {MessageContent}\r\n
// REPLY	REPLY {"OK"|"NOK"} IS {MessageContent}\r\n
// AUTH	AUTH {Username} AS {DisplayName} USING {Secret}\r\n
// JOIN	JOIN {ChannelID} AS {DisplayName}\r\n
// MSG	MSG FROM {DisplayName} IS {MessageContent}\r\n
// BYE	BYE\r\n
// CONFIRM	Unused in TCP

//ERR packet
TCPPacketErr::TCPPacketErr(std::string displayName, std::string messageContent)
{
    this->displayName = displayName;
    this->messageContent = messageContent;
}

PacketType TCPPacketErr::getType(){ return ERR; }

std::string TCPPacketErr::getData()
{
    //set data
    std::string output = "ERR FROM " + 
                         this->displayName +
                         " IS " +
                         this->messageContent +
                         "\r\n";

    //return data
    return output;
}

std::string TCPPacketErr::getMessage()
{
    return "ERR FROM " + this->displayName + ": " + this->messageContent + "\n";
}

//REPLY packet
TCPPacketReply::TCPPacketReply(bool is_ok, std::string messageContent)
{   
    this->is_ok = is_ok;
    this->messageContent = messageContent;
}

PacketType TCPPacketReply::getType(){ return REPLY; }

std::string TCPPacketReply::getData()
{
    //set data
    std::string ok = this->is_ok?"OK":"NOK";
    std::string output = "REPLY " + 
                         ok +
                         " IS " +
                         this->messageContent +
                         "\r\n";

    //return data
    return output;
}

bool TCPPacketReply::getValidity()
{
    return this->is_ok;
}

std::string TCPPacketReply::getMessage()
{
    std::string ok = this->is_ok?"Success: ":"Failure: ";
    return ok + this->messageContent + "\n";
}

//AUTH packet
TCPPacketAuth::TCPPacketAuth(std::string username, std::string displayName, std::string secret)
{
    this->username = username;
    this->displayName = displayName;
    this->secret = secret;
}

PacketType TCPPacketAuth::getType()
{
    return AUTH;
}

std::string TCPPacketAuth::getData()
{
    //create output data
    std::string output = "";

    //set data
    output.append("AUTH " + 
                  this->username +
                  " AS " + 
                  this->displayName +
                  " USING " +
                  this->secret +
                  "\r\n");

    //return data
    return output;
}

//JOIN packet
TCPPacketJoin::TCPPacketJoin(std::string chanelID, std::string displayName)
{
    this->chanelID = chanelID;
    this->displayName = displayName;
}

PacketType TCPPacketJoin::getType(){ return JOIN; }

std::string TCPPacketJoin::getData()
{
    //set data
    std::string output = "JOIN " + 
                         this->chanelID +
                         " AS " +
                         this->displayName +
                         "\r\n";

    //return data
    return output;
}

//MSG packet
TCPPacketMsg::TCPPacketMsg(std::string displayName, std::string messageContent)
{   
    this->displayName = displayName;
    this->messageContent = messageContent;
}

PacketType TCPPacketMsg::getType(){ return MSG; }

std::string TCPPacketMsg::getData()
{
    //set data
    std::string output = "MSG FROM " + 
                         this->displayName +
                         " IS " +
                         this->messageContent +
                         "\r\n";
    //return data
    return output;
}

std::string TCPPacketMsg::getMessage()
{
    return this->displayName + ": " + this->messageContent + "\n";
}

//BYE packet
PacketType TCPPacketBye::getType(){ return BYE; }

std::string TCPPacketBye::getData()
{
    //set data
    std::string output = "BYE\r\n";
    //return data
    return output;
}