#ifndef UDPCON
#define UDPCON

#include "TCPsocket.h"
#include "MyPacket.h"
#include <atomic>
#include <mutex>
#include <queue>
#include "TCPController.h"

class UDPController {
private:
    TCPSocket socket;
    int timeout;
    int retramsittions;
    int messageId;


    std::queue<struct SenderInput> commands;
    std::atomic<FSMStates> state;
    std::string displayName;
    std::mutex command_data_mux;
    std::mutex receiver_mux;

    std::string getPacketMessage(Packet* packet);
public:
    
    TCPController(const char server_ip[], int port, int timeout, int retramsittions);
    void sender();
    void receiver();
    void reader();

    void chat();
    void int_handler();
};

#endif