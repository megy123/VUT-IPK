#ifndef UDPCON
#define UDPCON

#include "socket.h"
#include "MyPacket.h"
#include <atomic>
#include <mutex>
#include <queue>
#include "TCPController.h"
#include <iostream>
#include "parser.h"
#include <sys/poll.h>

class UDPController {
private:
    Socket socket;
    int timeout;
    int retramsittions;
    int messageId;
    std::queue<PacketType> awaiting_packets;


    std::queue<struct SenderInput> commands;
    FSMStates state;
    std::string displayName;
    std::mutex command_data_mux;
    std::mutex receiver_mux;

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
    UDPController(const char server_ip[],const char port[], int timeout, int retramsittions);

    void chat();
    void int_handler();
};

#endif