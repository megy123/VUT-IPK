#include "Parser.h"
#include "TCPPackets.h"
#include <string.h>
#include <getopt.h>

std::string getDataToEnd(std::vector<std::string> data, int index)
{
    std::string output = "";

    for(int i = index; i < data.size(); i++)
    {
        output.append(data[i] + " ");
    }
    output.pop_back();
    return output;
}

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
        // output->push_back(COMAUTH);
        // output->push_back(command[1]);//username
        // output->push_back(command[2]);//secret
        // output->push_back(command[3]);//displayName
        out.command = command;
    }
    else if(command[0] == COMJOIN)
    {
        // output->push_back(COMJOIN);
        // output->push_back(command[1]);//chanelID
        out.command = command;
    }
    else if(command[0] == COMRENAME)
    {
        // output->push_back(COMRENAME);
        // output->push_back(command[1]);//displayName
        out.command = command;
    }
    else if(command[0] == COMHELP)
    {
        // output->push_back(COMHELP);
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
        if(packetData[1] == "OK")
        {
            return new TCPPacketReply(true, getDataToEnd(packetData, 3));
        }
        else if(packetData[1] == "NOK")
        {
            return new TCPPacketReply(false, getDataToEnd(packetData, 3));
        }
    }
    else if(packetData[0] == "ERR")
    {
        return new TCPPacketErr(packetData[2], getDataToEnd(packetData, 4));
    }
    else if(packetData[0] == "MSG")
    {
        return new TCPPacketMsg(packetData[2], getDataToEnd(packetData, 4));
    }
    else if(packetData[0] == "BYE")
    {
        return new TCPPacketBye();
    }
    return nullptr;
}
