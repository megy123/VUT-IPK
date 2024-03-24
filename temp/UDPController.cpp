#include "UDPController.h"
#include "UDPPackets.h"

UDPController::UDPController(const char server_ip[], int port, int timeout, int retramsittions)
{
    this->socket = TCPSocket(server_ip, port, SOCK_DGRAM);
    this->state = STATE_START;
    this->timeout = timeout;
    this->retramsittions = retramsittions;
    this->messageId = 0;
}

UDPController::chat()
{

}

void UDPController::int_handler()
{
    UDPPacketBye *packet = new UDPPacketBye();
    this->socket.sendPacket(packet);
}