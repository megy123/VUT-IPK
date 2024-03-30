#include "Parser.h"
#include "TCPPackets.h"
#include "UDPPackets.h"
#include <string.h>
#include <getopt.h>
#include <arpa/inet.h>

//helper functions
std::string getDataToEnd(std::vector<std::string> data, int index)
{
    std::string output = "";

    for(int i = index; i < data.size(); i++)// -2 is for \r\n
    {
        output.append(data[i] + " ");
    }
    output.pop_back();
    return output;
}

bool validName(std::string data)
{
    if(data.size() > 20)return false;
    for(int i=0;i<data.size();i++)
    {
        if(!( (data[i] >= 'A' && data[i] <= 'Z') ||
              (data[i] >= 'a' && data[i] <= 'z') ||
              (data[i] >= '0' && data[i] <= '9') ||
              (data[i] == '-') ))return false;
    }
    return true;
}

bool validDName(std::string data)
{
    if(data.size() > 20)return false;
    for(int i=0;i<data.size();i++)
    {
        if(!( (data[i] >= '\x21' && data[i] <= '\x7E')))return false;
    }
    return true;
}

bool validSecret(std::string data)
{
    if(data.size() > 128)return false;
    for(int i=0;i<data.size();i++)
    {
        if(!( (data[i] >= 'A' && data[i] <= 'Z') ||
              (data[i] >= 'a' && data[i] <= 'z') ||
              (data[i] >= '0' && data[i] <= '9') ||
              (data[i] == '-') ))return false;
    }
    return true;
}

bool validMessage(std::string data)
{
    if(data.size() > 1500)return false;
    for(int i=0;i<data.size();i++)
    {
        if(!( (data[i] >= '\x20' && data[i] <= '\x7E')))return false;
    }
    return true;
}

//parsing functions
void getCommand(std::queue<struct SenderInput> *output, std::string commStr)
{
    std::vector<std::string> command;
    int start, end;
    start = end = 0;
    char dl = ' ';
    //read command
    while ((start = commStr.find_first_not_of(dl, end)) != std::string::npos) {
        end = commStr.find(dl, start);
        command.push_back(commStr.substr(start, end - start));
    }

    SenderInput out;
    out.is_packet = false;
    

    //TODO: kontrola spravnosti parametrov
    //check commands
    if(command[0] == COMAUTH)
    {
        if(command.size() < 4)
        {
            std::cerr << "ERR: Invalid command argument!\n";
            exit(1);           
        }
        if( !(validName(command[1]) || validSecret(command[2]) || validDName(command[3])) )
        {
            std::cerr << "ERR: Invalid command argument!\n";
            exit(1);
        }
        out.command = command;
    }
    else if(command[0] == COMJOIN)
    {
        if(command.size() < 2)
        {
            std::cerr << "ERR: Invalid command argument!\n";
            exit(1);           
        }
        if( !(validName(command[1])) )
        {
            std::cerr << "ERR: Invalid command argument!\n";
            exit(1);
        }
        out.command = command;
    }
    else if(command[0] == COMRENAME)
    {
        if(command.size() < 2)
        {
            std::cerr << "ERR: Invalid command argument!\n";
            exit(1);           
        }
        if( !(validDName(command[1])) )
        {
            std::cerr << "ERR: Invalid command argument!\n";
            exit(1);
        }
        out.command = command;
    }
    else if(command[0] == COMHELP)
    {
        out.command = command;
    }
    else
    {
        std::vector<std::string> mess = {commStr};
        out.command = mess;
    }

    output->push(out);
}

Packet* resolvePacket(std::string receivedMsg)
{
    //remove whitespaces from the end
    while((receivedMsg.back() == '\n' || receivedMsg.back() == '\r') && !receivedMsg.empty())
    {
        receivedMsg.pop_back();
    }
    
    std::vector<std::string> packetData;
    int start, end;
    start = end = 0;
    char dl = ' ';

    //split packet
    while ((start = receivedMsg.find_first_not_of(dl, end)) != std::string::npos) {
        end = receivedMsg.find(dl, start);
        packetData.push_back(receivedMsg.substr(start, end - start));
    }


    if(packetData[0] == "REPLY")
    {
        if( !(validMessage(getDataToEnd(packetData, 3)) || packetData[2]=="IS" || packetData.size() < 3) )
        {
            std::cerr << "ERR: Received invalid data!\n";
            exit(1);
        }
        
        if(packetData[1] == "OK")
        {
            Packet *packet = new TCPPacketReply(true, getDataToEnd(packetData, 3));
            return packet;
        }
        else if(packetData[1] == "NOK")
        {
            Packet *packet = new TCPPacketReply(false, getDataToEnd(packetData, 3));
            return packet;
        }
    }
    else if(packetData[0] == "ERR")
    {
        if( !(validMessage(getDataToEnd(packetData, 4)) || validDName(packetData[2])
            || packetData[3]=="IS" || packetData[1]=="FROM" || packetData.size() < 4) )
        {
            std::cerr << "ERR: Received invalid data!\n";
            exit(1);
        }
        Packet *packet = new TCPPacketErr(packetData[2], getDataToEnd(packetData, 4));
        return packet;
    }
    else if(packetData[0] == "MSG")
    {
        if( !(validMessage(getDataToEnd(packetData, 4)) || validDName(packetData[2])
            || packetData[3]=="IS" || packetData[1]=="FROM" || packetData.size() < 4) )
        {
            std::cerr << "ERR: Received invalid data!\n";
            exit(1);
        }
        Packet *packet = new TCPPacketMsg(packetData[2], getDataToEnd(packetData, 4));
        return packet;
    }
    else if(packetData[0] == "BYE")
    {
        if(packetData.size() < 1)
        {
            std::cerr << "ERR: Received invalid data!\n";
            exit(1);            
        }
        Packet *packet = new TCPPacketBye();
        return packet;
    }

    //anything another is received
    std::cerr << "ERR: Received invalid data!\n";
    exit(1);
    return nullptr;
}

Packet* resolveUDPPacket(std::string receivedMsg)
{
    uint16_t msgId = 0;
    char splitMsg[2];
    splitMsg[0] = receivedMsg[1];
    splitMsg[1] = receivedMsg[2];

    std::memcpy(&msgId, &splitMsg, 2);
    msgId = ntohs(msgId);

    switch (receivedMsg[0])
    {
    case '\x00'://CONFIRM
        {
            return new UDPPacketConfirm(msgId);
            break;
        }
    case '\x01'://REPLY
        {
            bool is_ok;
            if(receivedMsg[3]=='\x00')
            {
                is_ok = false;
            }
            else if(receivedMsg[3]=='\x01')
            {
                is_ok = true;
            }
            else
            {
                std::cerr << "ERR: Invalid packet structure.\n";
                exit(1);
            }

            uint16_t refMsgId;
            std::string splitMessage = receivedMsg.substr(4,2);
            std::memcpy(&refMsgId, &splitMessage, 2);
            
            std::string Message;
            for(int i = 6;i<receivedMsg.size();i++)
            {

                Message += receivedMsg[i];
                if(receivedMsg[i] == '\x00')break;
            }

            return new UDPPacketReply(msgId, is_ok, refMsgId, Message);
            break;
        }
    case '\x02'://AUTH
        {
            std::string username;
            std::string displyName;
            std::string pass;
            int index = 3;

            while(receivedMsg[index]!='\x00')
            {
                
                username += receivedMsg[index];
                if(index == receivedMsg.size())
                {    
                    std::cerr << "ERR: Invalid packet structure.\n";
                    exit(1);
                }
                index++;
            }

            index++;
            while(receivedMsg[index]!='\x00')
            {
                
                displyName += receivedMsg[index];
                if(index == receivedMsg.size())
                {    
                    std::cerr << "ERR: Invalid packet structure.\n";
                    exit(1);
                }
                index++;
            }

            index++;
            while(receivedMsg[index]!='\x00')
            {
                
                pass += receivedMsg[index];
                if(index == receivedMsg.size())
                {    
                    std::cerr << "ERR: Invalid packet structure.\n";
                    exit(1);
                }
                index++;
            }
            
            return new UDPPacketAuth(msgId, username, displyName, pass);
            break;
        }
    case '\x03'://JOIN
        {
            std::string chanelID;
            std::string displayName;
            int index = 3;

            while(receivedMsg[index]!='\x00')
            {
                
                chanelID += receivedMsg[index];
                if(index == receivedMsg.size())
                {    
                    std::cerr << "ERR: Invalid packet structure.\n";
                    exit(1);
                }
                index++;
            }

            index++;
            while(receivedMsg[index]!='\x00')
            {
                
                displayName += receivedMsg[index];
                if(index == receivedMsg.size())
                {    
                    std::cerr << "ERR: Invalid packet structure.\n";
                    exit(1);
                }
                index++;
            }

            return new UDPPacketJoin(msgId, chanelID, displayName);
            break;
        }
    case '\x04'://MSG
        {
            std::string msgContent;
            std::string displayName;
            int index = 3;

            while(receivedMsg[index]!='\x00')
            {
                
                displayName += receivedMsg[index];
                if(index == receivedMsg.size())
                {    
                    std::cerr << "ERR: Invalid packet structure.\n";
                    exit(1);
                }
                index++;
            }

            index++;
            while(receivedMsg[index]!='\x00')
            {
                
                msgContent += receivedMsg[index];
                if(index == receivedMsg.size())
                {    
                    std::cerr << "ERR: Invalid packet structure.\n";
                    exit(1);
                }
                index++;
            }

            return new UDPPacketMsg(msgId, displayName, msgContent);
            break;
        }
    case '\xFE'://ERR
        {
            std::string msgContent;
            std::string displayName;
            int index = 3;

            while(receivedMsg[index]!='\x00')
            {
                
                displayName += receivedMsg[index];
                if(index == receivedMsg.size())
                {    
                    std::cerr << "ERR: Invalid packet structure.\n";
                    exit(1);
                }
                index++;
            }

            index++;
            while(receivedMsg[index]!='\x00')
            {
                
                msgContent += receivedMsg[index];
                if(index == receivedMsg.size())
                {    
                    std::cerr << "ERR: Invalid packet structure.\n";
                    exit(1);
                }
                index++;
            }

            return new UDPPacketErr(msgId, displayName, msgContent);
            break;
        }
    case '\xFF'://BYE
        {
            return new UDPPacketBye(msgId);
            break;
        }
    default:
        return nullptr;
        break;
    }
}