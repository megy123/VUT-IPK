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
    return "";
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
                            this->displayName = command.command[2];
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
                            this->displayName = command.command[2];
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
                            TCPPacketJoin *packet = new TCPPacketJoin(command.command[1], this->displayName);
                            this->socket.sendPacket(packet);
                        }
                        else if(command.command[0] == COMRENAME)
                        {
                            this->displayName = command.command[1];
                        }
                        else if(command.command[0] == COMHELP)
                        {
                            std::cout << helpMessage;
                        }
                        else
                        {
                            //message
                            TCPPacketMsg *packet = new TCPPacketMsg(this->displayName, command.command[0]);
                            this->socket.sendPacket(packet);
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
                    Packet* packet = resolvePacket(data);

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
                        this->commands.push(newInput);
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
                    Packet* packet = resolvePacket(data);

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
                        this->commands.push(newInput);
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
                        this->commands.push(newInput);
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
    // std::thread rec_thread(&TCPController::receiver, this);
    // std::thread send_thread(&TCPController::sender, this);
    // std::thread read_thread(&TCPController::reader, this);

    // rec_thread.join();
    // send_thread.join();
    // read_thread.join();

    while(this->state != STATE_END)
    {
        if(true)
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
                this->state = STATE_ERROR;
            }
            else
            {
                if(command.command[0] == COMAUTH)
                {
                    handle_command(command);
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
            //handle awaiting packets
            if(!awaiting_packets.empty())
            {
                command.packet->getType() != this->awaiting_packets.front();
                this->awaiting_packets.pop();
                std::cerr << "ERR: Invalid packet received. Awaiting another packet type!\n";
                this->state = STATE_ERROR;
                return;
            }

            if(command.packet->getType() == REPLY)
            {
                handle_command(command);
                if(dynamic_cast<TCPPacketReply*>(command.packet)->getValidity())
                {
                    this->state = STATE_OPEN;
                }
            }
            else if(command.packet->getType() == ERR)
            {
                handle_command(command);
                TCPPacketBye* packet = new TCPPacketBye();
                this->socket.sendPacket(packet);
                this->state = STATE_END;
            }
            else
            {
                std::cerr << "ERR: Invalid packet received.\n";
                this->state = STATE_ERROR;
            }

        }
        else
        {
            if(command.command[0] == COMAUTH)
            {
                handle_command(command);
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
            //handle awaiting packets
            if(!awaiting_packets.empty())
            {
                command.packet->getType() != this->awaiting_packets.front();
                this->awaiting_packets.pop();
                std::cerr << "ERR: Invalid packet received. Awaiting another packet type!\n";
                this->state = STATE_ERROR;
                return;
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
                std::cerr << "ERR: You are already authenificated!\n";
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
        std::cerr << dynamic_cast<TCPPacketMsg*>(command.packet)->getMessage();
    }
    else if(command.packet->getType() == ERR)
    {
        std::cerr << dynamic_cast<TCPPacketErr*>(command.packet)->getMessage();
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
        std::cout << "helpMessage\n";
    }
    else
    {
        //message
        TCPPacketMsg *packet = new TCPPacketMsg(this->displayName, command.command[0]);
        this->socket.sendPacket(packet);
    }
}

void TCPController::read_from_stdin()
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
        getCommand(&this->commands, commStr);
    }
    else if(ret == 0){}
    else
    {
        std::cerr << "Error while reading from stdin\n";
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

// /auth xsajko01 otravnyPomaranc 9c7150a2-15b7-4dbc-8ee4-b14a25d93257
// REPLY OK IS Auth success.
// MSG FROM server_user IS sa uvedooom!!!
// ERR FROM server_user IS {MessageContent}