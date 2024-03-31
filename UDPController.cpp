#include "UDPController.h"
#include "UDPPackets.h"
#include <sys/socket.h>

UDPController::UDPController(const char server_ip[],const char port[], int timeout, int retramsittions)
{
    this->ip = server_ip;
    this->socket = Socket(server_ip, port, SOCK_DGRAM);
    this->state = STATE_START;
    this->timeout = timeout;
    this->retramsittions = retramsittions;
    this->messageId = 0;
}


std::string UDPController::getPacketMessage(Packet* packet)
{
    switch(packet->getType())
    {
    case ERR:
        {
            return dynamic_cast<UDPPacketErr*>(packet)->getMessage();
            break;
        }
    case REPLY:
        {
            return dynamic_cast<UDPPacketReply*>(packet)->getMessage();
            break;
        }
    case MSG:
        {
            return dynamic_cast<UDPPacketMsg*>(packet)->getMessage();
            break;
        }
    }
    return "";
}

ControllerType UDPController::getType()
{
    return UDPCONT;
}

void UDPController::chat()
{
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
void UDPController::start_events()
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

void UDPController::auth_events()
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
                if(command.packet->getType() == this->awaiting_packets.front() || command.packet->getType() == ERR)
                {
                    this->awaiting_packets.pop();                
                }
                else
                {
                    std::cerr << "ERR: Invalid packet received. Awaiting another packet type!\n";
                    this->state = STATE_ERROR;
                    return;
                }
            }

            if(command.packet->getType() == REPLY)
            {
                handle_packet(command);
                if(dynamic_cast<UDPPacketReply*>(command.packet)->getValidity())
                {
                    this->state = STATE_OPEN;
                }
            }
            else if(command.packet->getType() == ERR)
            {
                handle_packet(command);
                UDPPacketBye* packet = new UDPPacketBye(this->messageId++);
                this->socket.sendPacket(packet, this->retramsittions, this->timeout);
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

void UDPController::open_events()
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
                if(command.packet->getType() == this->awaiting_packets.front() || command.packet->getType() == ERR)
                {
                    this->awaiting_packets.pop();
                }
                else
                {
                    std::cerr << "ERR: Invalid packet received. Awaiting another packet type!\n";
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

void UDPController::error_events()
{
    UDPPacketBye *packet = new UDPPacketBye(this->messageId++);
    this->socket.sendPacket(packet, this->retramsittions, this->timeout);
    this->state = STATE_END;
}

//helping methods
void UDPController::handle_packet(SenderInput command)
{
    uint16_t messageId = 0;
    if(command.packet->getType() == REPLY)
    {
        std::cerr << dynamic_cast<UDPPacketReply*>(command.packet)->getMessage();
        messageId = dynamic_cast<UDPPacketReply*>(command.packet)->getMessageId();
    }
    else if(command.packet->getType() == MSG)
    {
        std::cout << dynamic_cast<UDPPacketMsg*>(command.packet)->getMessage();
        messageId = dynamic_cast<UDPPacketMsg*>(command.packet)->getMessageId();
    }
    else if(command.packet->getType() == ERR)
    {
        std::cerr << dynamic_cast<UDPPacketErr*>(command.packet)->getMessage();
        messageId = dynamic_cast<UDPPacketErr*>(command.packet)->getMessageId();
    }
    else if(command.packet->getType() == BYE)
    {
        int_handler();
        exit(0);
    }
    this->socket.sendPacket(new UDPPacketConfirm(messageId), this->retramsittions, this->timeout);
}

void UDPController::handle_command(SenderInput command)
{
    if(command.is_packet)
    {
        std::cerr << "ERR: Invalid command\n";
        exit(1);
    }

    if(command.command[0] == COMAUTH)
    {
        UDPPacketAuth *packet = new UDPPacketAuth(this->messageId++, command.command[1], command.command[2], command.command[3]);
        this->displayName = command.command[3];
        if(this->socket.sendPacket(packet, this->retramsittions, this->timeout))
        {
            std::cerr << "ERR: Not received Confirm packet.\n";
            exit(1);
        }
    }
    else if(command.command[0] == COMJOIN)
    {
        UDPPacketJoin *packet = new UDPPacketJoin(this->messageId++, command.command[1], this->displayName);
        if(this->socket.sendPacket(packet, this->retramsittions, this->timeout))
        {
            std::cerr << "ERR: Not received Confirm packet.\n";
            exit(1);
        }
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
        UDPPacketMsg *packet = new UDPPacketMsg(this->messageId++, this->displayName, command.command[0]);
        if(this->socket.sendPacket(packet, this->retramsittions, this->timeout))
        {
            std::cerr << "ERR: Not received Confirm packet.\n";
            exit(1);
        }
    }
}

void UDPController::read_from_stdin()
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
        std::cerr << "Error while reading from stdin\n";
        exit(1);
    }
}

void UDPController::read_from_socket()
{
    if(this->socket.dataAvailable() > 0)
    {
        std::string data = this->socket.receiveData();
        Packet* packet = resolveUDPPacket(data);

        SenderInput newInput;
        newInput.is_packet = true;
        newInput.packet = packet;

        this->commands.push(newInput);
    }
}

void UDPController::int_handler(){
    UDPPacketBye *packet = new UDPPacketBye(this->messageId++);
    this->socket.sendPacket(packet, this->retramsittions, this->timeout);
}