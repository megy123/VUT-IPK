#ifndef TCPCON
#define TCPCON

#include "TCPsocket.h"
#include "Packet.h"
#include <atomic>
#include <mutex>
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

class TCPController {
private:
    TCPSocket socket;
    std::queue<struct SenderInput> commands;
    std::atomic<FSMStates> state;
    std::mutex command_data_mux;
    std::mutex receiver_mux;

    std::string getPacketMessage(Packet* packet);
public:
    
    TCPController(const char server_ip[], int port);
    void sender();
    void receiver();
    void reader();

    void chat();
};

#endif