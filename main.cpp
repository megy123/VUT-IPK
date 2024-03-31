/*
Project:    IPK 1. projekt
File:       main.cpp
Authors:    Dominik Sajko (xsajko01)
Date:       31.03.2024
*/
#include <stdio.h>
#include <stdlib.h>
#include "TCPController.h"
#include "UDPController.h"
#include "Controller.h"
#include "Parser.h"
#include <signal.h>

Controller* controller;

void handle(int i)
{
    if(controller->getType() == TCPCONT)
    {
        dynamic_cast<TCPController*>(controller)->int_handler();
    }
    else
    {
        dynamic_cast<UDPController*>(controller)->int_handler();
    }
    exit(i);
}

int main(int argc, char *argv[])
{
    //--ARGUMENT PARSING--
    //set default arguments
    struct ParsedArgs args;
    int c;
    args.port = "4567";
    args.timeout = 250;
    args.transmissions = 3;

    //get args
    while ((c = getopt(argc, argv, "t:s:p:d:r:h")) != -1)
    {
        switch(c)
        {
        case 't':
            if(strcmp(optarg, "tcp") == 0)
            {
                args.protocol = TCP;
            }
            else if(strcmp(optarg, "udp") == 0)
            {
                args.protocol = UDP;
            }
            else
            {
                std::cerr << "Invalid protocol specified.\n";
                exit(1);
            }
            break;
        case 's':
            args.ip = optarg;
            break;
        case 'p':
            args.port = optarg;
            break;
        case 'd':
            args.timeout = std::stoi(optarg);
            break;
        case 'r':
            args.transmissions = std::stoi(optarg);
            break;
        case '?':
        default:
            //TODO: dopíš help
            std::cout << "this is help\n";
            exit(1);
        }
    }

    //check for mandatory arguments
    int check = 0;
    for(int i = 0; i < argc; i++)
    {
        if((strcmp(argv[i], "-t") == 0) || (strcmp(argv[i], "-s") == 0))check++;
    }
    if(check != 2)
    {
        std::cerr << "You need specify -t and -s arguments.\n";
        exit(1);
    }

    //--PROGRAM CONTROL--

    //create controller
    if(args.protocol == TCP)
    {
        controller = new TCPController(args.ip.c_str(), args.port.c_str());
        
    }
    else
    {
        controller = new UDPController(args.ip.c_str(), args.port.c_str(), 250, 3);
    }
    
    //Assign interrupt handler
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = handle;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);    

    //start FSM
    if(args.protocol == TCP)
    {
        dynamic_cast<TCPController*>(controller)->chat();
    }
    else
    {
        dynamic_cast<UDPController*>(controller)->chat();
    }

    return 0;
}

