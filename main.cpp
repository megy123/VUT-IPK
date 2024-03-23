#include <stdio.h>
#include <stdlib.h>
#include "TCPController.h"
#include "Parser.h"
#include <signal.h>

TCPController* controller;

void handle(int i)
{
    controller->int_handler();
    exit(i);
}

int main(int argc, char *argv[])
{
    // --ARGUMENT PARSING--
    // //set default arguments
    // struct ParsedArgs args;
    // int c;
    // args.port = 4567;
    // args.timeout = 250;
    // args.transmissions = 3;

    // //get args
    // while ((c = getopt(argc, argv, "t:s:p:d:r:h")) != -1)
    // {
    //     switch(c)
    //     {
    //     case 't':
    //         if(strcmp(optarg, "tcp") == 0)
    //         {
    //             args.protocol = TCP;
    //         }
    //         else if(strcmp(optarg, "udp") == 0)
    //         {
    //             args.protocol = UDP;
    //         }
    //         else
    //         {
    //             std::cerr << "Invalid protocol specified.\n";
    //             exit(1);
    //         }
    //         break;
    //     case 's':
    //         args.ip = optarg;
    //         break;
    //     case 'p':
    //         args.port = std::stoi(optarg); // TODO: stoi errory
    //         break;
    //     case 'd':
    //         args.timeout = std::stoi(optarg);
    //         break;
    //     case 'r':
    //         args.transmissions = std::stoi(optarg);
    //         break;
    //     case '?':
    //     default:
    //         //TODO: dopíš help
    //         std::cout << "this is help\n";
    //         exit(1);
    //     }
    // }

    // //check for mandatory arguments
    // int check = 0;
    // for(int i = 0; i < argc; i++)
    // {
    //     if((strcmp(argv[i], "-t") == 0) || (strcmp(argv[i], "-s") == 0))check++;
    // }
    // if(check != 2)
    // {
    //     std::cerr << "You need specify -t and -s arguments.\n";
    //     exit(1);
    // }

    // std::cout << "ip: \t\t" << args.ip << "\n";
    // std::cout << "port: \t\t" << args.port << "\n";
    // std::cout << "protocol: \t\t" << args.protocol << "\n";
    // std::cout << "timeout: \t\t" << args.timeout << "\n";
    // std::cout << "transmision: \t\t" << args.transmissions << "\n";



    //resolve ip from domain
    

    //TCP connection

    //147.229.8.244 -- anton5.fit.vutbr.cz
    controller = new TCPController("127.0.0.1", 4567);
    
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = handle;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    
    controller->chat();
    


    return 0;
}

