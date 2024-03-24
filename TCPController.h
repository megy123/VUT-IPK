#ifndef TCPCON
#define TCPCON

#include "socket.h"
#include "MyPacket.h"
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
    Socket socket;
    std::queue<struct SenderInput> commands;
    std::atomic<FSMStates> state;
    std::string displayName;
    std::mutex command_data_mux;
    std::mutex receiver_mux;

    std::string getPacketMessage(Packet* packet);
public:
    
    TCPController(const char server_ip[], const char port[]);
    void sender();
    void receiver();
    void reader();

    void chat();
    void int_handler();
};

#endif