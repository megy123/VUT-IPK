#include "TCPController.h"
#include <iostream>
#include <string>
#include <vector>
#include "Parser.h"
#include "TCPPackets.h"
#include <sys/poll.h>
#include <chrono>
#include <thread>
#include <utility>



TCPController::TCPController(const char server_ip[], int port)
{
    this->socket = TCPSocket(server_ip, port);
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
    case AUTH:
        {
            return "";
            break;
        }
    case JOIN:
        {
            return "";
            break;
        }
    case MSG:
        {
            return dynamic_cast<TCPPacketMsg*>(packet)->getMessage();
            break;
        }
    case BYE:
        {   
            return "";
            break;
        }
    }
}

void TCPController::sender()
{
    std::string helpMessage = "HELP!\n";

    while(this->state != STATE_END)
    {
        command_data_mux.lock();
        if(!this->commands.empty() || this->state == STATE_ERROR)
        {   
            struct SenderInput command;
            if(this->state != STATE_ERROR)
            {
                command = this->commands.front();
                this->commands.pop();
            }

            switch(this->state)
            {
                case STATE_START:
                {
                    if(command.is_packet)
                    {
                        std::cerr << "ERR: Invalid packet to send.\n";
                        this->state = STATE_ERROR;
                    }
                    else
                    {
                        if(command.command[0] == COMAUTH)
                        {
                            TCPPacketAuth *packet = new TCPPacketAuth(command.command[1], command.command[2], command.command[3]);
                            this->socket.sendPacket(packet);
                            state = STATE_AUTH;
                        }
                        else if(command.command[0] == COMHELP)
                        {
                            std::cout << helpMessage;
                        }
                        else
                        {
                            std::cerr << "ERR: You need to authenicate first using \"/auth {Username} {Secret} {DisplayName}\"\n";
                        }
                    }
                    break;
                }
                case STATE_AUTH:
                {
                    if(command.is_packet)
                    {
                        if(command.packet->getType() == AUTH)
                        {
                            this->socket.sendPacket(command.packet);
                        }
                        else if(command.packet->getType() == BYE)
                        {
                            this->socket.sendPacket(command.packet);
                            this->state = STATE_END;
                        }
                        else
                        {
                            std::cerr << "ERR: Invalid packet to send.\n";
                            this->state = STATE_ERROR;
                        }
                    }
                    else
                    {
                        if(command.command[0] == COMAUTH)
                        {
                            TCPPacketAuth *packet = new TCPPacketAuth(command.command[1], command.command[2], command.command[3]);
                            this->socket.sendPacket(packet);
                            state = STATE_AUTH;
                        }
                        else if(command.command[0] == COMHELP)
                        {
                            std::cout << helpMessage;
                        }
                        else
                        {
                            std::cerr << "ERR: You need to authenicate first using \"/auth {Username} {Secret} {DisplayName}\"\n";
                        }
                    }
                    break;
                }
                case STATE_OPEN:
                {
                    if(command.is_packet)
                    {
                        if(command.packet->getType() == JOIN)
                        {
                            this->socket.sendPacket(command.packet);
                        }
                        else if(command.packet->getType() == MSG)
                        {
                            this->socket.sendPacket(command.packet);
                        }
                        else if(command.packet->getType() == ERR)
                        {
                            this->socket.sendPacket(command.packet);
                            this->state = STATE_ERROR;
                        }
                        else if(command.packet->getType() == BYE)
                        {
                            this->socket.sendPacket(command.packet);
                            this->state = STATE_END;
                        }
                        else
                        {
                            std::cerr << "ERR: Invalid packet to send.\n";
                            this->state = STATE_ERROR;
                        }
                    }
                    else
                    {
                        if(command.command[0] == COMAUTH)
                        {
                            TCPPacketAuth *packet = new TCPPacketAuth(command.command[1], command.command[2], command.command[3]);
                            this->socket.sendPacket(packet);
                        }
                        else if(command.command[0] == COMJOIN)
                        {
                            TCPPacketJoin *packet = new TCPPacketJoin(command.command[1], command.command[2]);
                            this->socket.sendPacket(packet);
                        }
                        else if(command.command[0] == COMRENAME)
                        {
                            //TODO: nwm to to ma byt
                        }
                        else if(command.command[0] == COMHELP)
                        {
                            std::cout << helpMessage;
                        }
                        else
                        {
                            std::cerr << "ERR: Invalid command\"\n";
                        }
                    }                
                    break;
                }                            
                case STATE_ERROR:
                {
                    TCPPacketBye *packet = new TCPPacketBye();
                    this->socket.sendPacket(packet);
                    this->state = STATE_END;
                    break;
                }
                case STATE_END:
                {
                    break;
                }
            }
        }
        command_data_mux.unlock();
    }
}

void TCPController::receiver()
{
    while(this->state != STATE_END)
    {
        if(this->socket.dataAvailable() > 0)
        {
            switch(this->state)
            {
                case STATE_START:
                {
                    this->state = STATE_ERROR;
                    break;
                }
                case STATE_AUTH:
                {
                    std::string data = this->socket.receiveData();
                    Packet* packet = getPacket(data);

                    if(packet->getType() == REPLY)
                    {
                        std::cerr << dynamic_cast<TCPPacketReply*>(packet)->getMessage();
                        if(dynamic_cast<TCPPacketReply*>(packet)->getValidity())
                        {
                            this->state = STATE_OPEN;
                        }
                    }
                    else if(packet->getType() == ERR)
                    {
                        std::cerr << dynamic_cast<TCPPacketErr*>(packet)->getMessage();
                        SenderInput newInput;
                        newInput.is_packet = true;
                        newInput.packet = new TCPPacketBye();
                        this->commands.push()
                    }
                    else
                    {
                        this->state = STATE_ERROR;
                    }
                    break;
                }
                case STATE_OPEN:
                {
                    std::string data = this->socket.receiveData();
                    Packet* packet = getPacket(data);

                    if(packet->getType() == MSG)
                    {
                        std::cerr << dynamic_cast<TCPPacketMsg*>(packet)->getMessage();
                    }
                    else if(packet->getType() == REPLY)
                    {
                        std::cerr << dynamic_cast<TCPPacketReply*>(packet)->getMessage();
                    }
                    else if(packet->getType() == ERR)
                    {
                        std::cerr << dynamic_cast<TCPPacketErr*>(packet)->getMessage();
                        SenderInput newInput;
                        newInput.is_packet = true;
                        newInput.packet = new TCPPacketBye();
                        this->commands.push()
                    }
                    else if(packet->getType() == BYE)
                    {
                        this->state = STATE_END;
                    }
                    else
                    {
                        SenderInput newInput;
                        newInput.is_packet = true;
                        newInput.packet = new TCPPacketBye();
                        this->commands.push()
                    }                 
                    break;
                }                            
                case STATE_ERROR:
                {
                    break;
                }
                case STATE_END:
                {
                    break;
                }
            }
        }
    }
}

void TCPController::reader()
{
    //get command
    while(this->state != STATE_END)
    {
        std::string commStr;
        struct pollfd fds;
        int ret;
        fds.fd = 0; /* this is STDIN */
        fds.events = POLLIN;
        ret = poll(&fds, 1, 0);
        if(ret == 1)
        {
            getline(std::cin, commStr);//TODO: co ak prazdny string
            command_data_mux.lock();
            getCommand(&this->commands, commStr);
            command_data_mux.unlock();
        }
        else if(ret == 0){}
        else
        {
            printf("Error\n");
            break;
        }
    }
}

void TCPController::chat()
{
    //std::thread rec_thread(&TCPController::receiver, this);
    std::thread send_thread(&TCPController::sender, this);
    std::thread read_thread(&TCPController::reader, this);

    //rec_thread.join();
    send_thread.join();
    read_thread.join();
    

    //TCPPacketAuth *packet = new TCPPacketAuth("xsajko01", "otravnyPomaranc", "9c7150a2-15b7-4dbc-8ee4-b14a25d93257");
    //this->socket.sendPacket(packet);
    std::cout << "chat\n";
}

// /auth xsajko01 otravnyPomaranc 9c7150a2-15b7-4dbc-8ee4-b14a25d93257
// REPLY OK IS Auth success.
// ERR FROM {DisplayName} IS {MessageContent}