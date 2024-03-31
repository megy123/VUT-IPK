/*
Project:    IPK 1. projekt
File:       TCPController.h
Authors:    Dominik Sajko (xsajko01)
Date:       31.03.2024
*/
#ifndef TCPCON
#define TCPCON

#include "Controller.h"
#include "socket.h"
#include "MyPacket.h"
#include <queue>

enum FSMStates{
    STATE_START,
    STATE_AUTH,
    STATE_OPEN,
    STATE_ERROR,
    STATE_END
};

struct SenderInput{
    std::vector<std::string> command;
    Packet* packet;
    bool is_packet;
};

class TCPController : public Controller{
private:
    Socket socket;
    std::queue<struct SenderInput> commands;
    std::queue<PacketType> awaiting_packets;
    FSMStates state;

    std::string displayName;

    std::string getPacketMessage(Packet* packet);
    void read_from_stdin();
    void read_from_socket();
    void handle_command(SenderInput command);
    void handle_packet(SenderInput command);

    void start_events();
    void auth_events();
    void open_events();
    void error_events();
public:
    TCPController(const char server_ip[], const char port[]);

    void chat();
    void int_handler();
    virtual ControllerType getType();
};

#endif