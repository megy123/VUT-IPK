#ifndef PARSER
#define PARSER

#include <iostream>
#include <vector>
#include <string>
#include <bits/stdc++.h>
#include "MyPacket.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <queue>
#include "TCPController.h"

const char COMAUTH[6] = "/auth";
const char COMJOIN[6] = "/join";
const char COMRENAME[8] = "/rename";
const char COMHELP[6] = "/help";

enum Protocol{
    TCP,
    UDP
};

struct ParsedArgs {
  Protocol protocol;
  std::string ip;
  std::string port;
  int timeout;
  int transmissions;
};

//methods for input handling
void getCommand(std::queue<struct SenderInput> *output, std::string commStr);

Packet* resolvePacket(std::string receivedMsg);

Packet* resolveUDPPacket(std::string receivedMsg);

#endif