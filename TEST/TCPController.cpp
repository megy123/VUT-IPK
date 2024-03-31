/*
Project:    IPK 1. projekt
File:       TCPController.cpp
Authors:    Dominik Sajko (xsajko01)
Date:       31.03.2024
*/
#include "TCPController.h"
#include <iostream>
#include <string>
#include <vector>
#include "Parser.h"
#include "TCPPackets.h"
#include <sys/poll.h>
#include <sys/socket.h>

TCPController::TCPController(const char server_ip[], const char port[])
{
    this->socket = Socket(server_ip, port, SOCK_STREAM);
    this->state = STATE_START;
}

std::string TCPController::getPacketMessage(Packet* packet)
{
    switch(packet->getType())
    {
    case ERR:
        {
            return dynamic_cast<TCPPacketErr*>(packet)->getMessage();
            break;
        }
    case REPLY:
        {
            return dynamic_cast<TCPPacketReply*>(packet)->getMessage();
            break;
        }
    case MSG:
        {
            return dynamic_cast<TCPPacketMsg*>(packet)->getMessage();
            break;
        }
    }
    return "";
}

ControllerType TCPController::getType()
{
    return TCPCONT;
}

void TCPController::chat()
{
    while(this->state != STATE_END)
    {
        //get input
        if(this->awaiting_packets.empty())read_from_stdin();
        read_from_socket();

        //fsm
        switch(this->state)
        {
            case STATE_START:
            {
                start_events();
                break;
            }
            case STATE_AUTH:
            {
                auth_events();
                break;
            }
            case STATE_OPEN:
            {                
                open_events();
                break;
            }                            
            case STATE_ERROR:
            {
                error_events();
                break;
            }
            case STATE_END:
            {
                break;
            }
        }
    }

}

//FSM states
void TCPController::start_events()
{
    if(this->awaiting_packets.empty())
    {
        if(!commands.empty())
        {
            //get command
            struct SenderInput command;
            command = this->commands.front();
            this->commands.pop();

            //handle command
            if(command.is_packet)
            {
                std::cerr << "ERR: Received invalid packet.\n";
                TCPPacketErr* packet = new TCPPacketErr(this->displayName, "Invalid packet received.");
                this->socket.sendPacket(packet);
                this->state = STATE_ERROR;
            }
            else
            {
                if(command.command[0] == COMAUTH)
                {
                    handle_command(command);
                    this->awaiting_packets.push(REPLY);
                    this->state = STATE_AUTH;
                }
                else if(command.command[0] == COMHELP)
                {
                    handle_command(command);
                }
                else
                {
                    std::cerr << "ERR: You need to authenicate first using \"/auth {Username} {Secret} {DisplayName}\"\n";
                }
            }
        }
    }
    else
    {
        std::cerr << "ERR: Awaiting packet in invalid FSM state.\n";
        exit(1);
    }
}

void TCPController::auth_events()
{
    if(!commands.empty())
    {
        //get command
        struct SenderInput command;
        command = this->commands.front();
        this->commands.pop();

        if(command.is_packet)
        {
            //invalid command
            if(command.packet == nullptr)
            {
                std::cerr << "ERR: Invalid packet received.\n";
                TCPPacketErr* packet = new TCPPacketErr(this->displayName, "Invalid packet received.");
                this->socket.sendPacket(packet);
                this->state = STATE_ERROR;
                return;
            }

            //handle awaiting packets
            if(!awaiting_packets.empty())
            {
                if(command.packet->getType() == this->awaiting_packets.front() || command.packet->getType() == ERR)
                {
                    this->awaiting_packets.pop();                
                }
                else
                {
                    std::cerr << "ERR: Invalid packet received. Awaiting another packet type!\n";
                    TCPPacketErr* packet = new TCPPacketErr(this->displayName, "Invalid packet received.");
                    this->socket.sendPacket(packet);
                    this->state = STATE_ERROR;
                    return;
                }
            }

            if(command.packet->getType() == REPLY)
            {
                handle_packet(command);
                if(dynamic_cast<TCPPacketReply*>(command.packet)->getValidity())
                {
                    this->state = STATE_OPEN;
                }
            }
            else if(command.packet->getType() == ERR)
            {
                handle_packet(command);
                TCPPacketBye* packet = new TCPPacketBye();
                this->socket.sendPacket(packet);
                this->state = STATE_END;
            }
        }
        else
        {
            if(command.command[0] == COMAUTH)
            {
                handle_command(command);
                this->awaiting_packets.push(REPLY);
                state = STATE_AUTH;
            }
            else if(command.command[0] == COMHELP)
            {
                handle_command(command);
            }
            else
            {
                std::cerr << "ERR: You need to authenicate first using \"/auth {Username} {Secret} {DisplayName}\"\n";
            }
        }
    }
}

void TCPController::open_events()
{
    if(!commands.empty())
    {
        //get command
        struct SenderInput command;
        command = this->commands.front();
        this->commands.pop();

        if(command.is_packet)
        {
            //invalid command
            if(command.packet == nullptr)
            {
                std::cerr << "ERR: Invalid packet received.\n";
                TCPPacketErr* packet = new TCPPacketErr(this->displayName, "Invalid packet received.");
                this->socket.sendPacket(packet);
                this->state = STATE_ERROR;
                return;
            }

            //handle awaiting packets
            if(!awaiting_packets.empty())
            {
                if(command.packet->getType() == this->awaiting_packets.front() || command.packet->getType() == ERR)
                {
                    this->awaiting_packets.pop();
                }
                else
                {
                    std::cerr << "ERR: Invalid packet received. Awaiting another packet type!\n";
                    TCPPacketErr* packet = new TCPPacketErr(this->displayName, "Invalid packet received.");
                    this->socket.sendPacket(packet);
                    this->state = STATE_ERROR;
                    return;
                }
            }

            if(command.packet->getType() == REPLY)
            {
                handle_packet(command);
            }
            else if(command.packet->getType() == MSG)
            {
                handle_packet(command);
            }
            else if(command.packet->getType() == ERR)
            {
                handle_packet(command);
                this->state = STATE_ERROR;
            }
            else if(command.packet->getType() == BYE)
            {
                handle_packet(command);
                this->state = STATE_END;
            }
        }
        else
        {
            if(command.command[0] == COMAUTH)
            {
                std::cerr << "ERR: You are already authenificated!\n";
            }
            else if(command.command[0] == COMJOIN)
            {
                handle_command(command);
                this->awaiting_packets.push(REPLY);
            }
            else
            {
                //message
                handle_command(command);
            }
        }  

    }
}

void TCPController::error_events()
{
    TCPPacketBye *packet = new TCPPacketBye();
    this->socket.sendPacket(packet);
    this->state = STATE_END;
}

//helping methods
void TCPController::handle_packet(SenderInput command)
{
    if(command.packet->getType() == REPLY)
    {
        std::cerr << dynamic_cast<TCPPacketReply*>(command.packet)->getMessage();
    }
    else if(command.packet->getType() == MSG)
    {
        std::cout << dynamic_cast<TCPPacketMsg*>(command.packet)->getMessage();
    }
    else if(command.packet->getType() == ERR)
    {
        std::cerr << dynamic_cast<TCPPacketErr*>(command.packet)->getMessage();
    }
    else if(command.packet->getType() == BYE)
    {
        int_handler();
        exit(0);
    }
}

void TCPController::handle_command(SenderInput command)
{
    if(command.is_packet)
    {
        std::cerr << "ERR: Invalid command\n";
        exit(1);
    }

    if(command.command[0] == COMAUTH)
    {
        TCPPacketAuth *packet = new TCPPacketAuth(command.command[1], command.command[2], command.command[3]);
        this->displayName = command.command[3];
        this->socket.sendPacket(packet);
    }
    else if(command.command[0] == COMJOIN)
    {
        TCPPacketJoin *packet = new TCPPacketJoin(command.command[1], this->displayName);
        this->socket.sendPacket(packet);
    }
    else if(command.command[0] == COMRENAME)
    {
        this->displayName = command.command[1];
    }
    else if(command.command[0] == COMHELP)
    {
        std::cout << "Commands\n"
                  << "/auth {Username} {Secret} {DisplayName}\n"
                  << "/join {ChannelID}\n"
                  << "/rename {DisplayName}\n"
                  << "/help";
    }
    else
    {
        //message packet
        TCPPacketMsg *packet = new TCPPacketMsg(this->displayName, command.command[0]);
        this->socket.sendPacket(packet);
    }
}

void TCPController::read_from_stdin()
{
    std::string commStr;

    struct pollfd fds;
    int ret;
    fds.fd = 0; // 0 = stdin
    fds.events = POLLIN;
    ret = poll(&fds, 1, 0);
    if(ret == 1)
    {
        getline(std::cin, commStr);
        if(std::cin.eof())//Ctrl D iterrupt
        {
            int_handler();
            exit(0);
        }
        getCommand(&this->commands, commStr);
    }
    else if(ret == 0){}
    else
    {
        std::cerr << "ERR: Error while reading from stdin\n";
        exit(1);
    }
}

void TCPController::read_from_socket()
{
    if(this->socket.dataAvailable() > 0)
    {
        std::string data = this->socket.receiveData();
        Packet* packet = resolvePacket(data);

        SenderInput newInput;
        newInput.is_packet = true;
        newInput.packet = packet;

        this->commands.push(newInput);
    }
}

void TCPController::int_handler()
{
    TCPPacketBye *packet = new TCPPacketBye();
    this->socket.sendPacket(packet);
}