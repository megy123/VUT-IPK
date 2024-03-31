/*
Project:    IPK 1. projekt
File:       Controller.h
Authors:    Dominik Sajko (xsajko01)
Date:       31.03.2024
*/
#ifndef CONTROLLER
#define CONTROLLER

enum ControllerType{
    NONECON,
    UDPCONT,
    TCPCONT
};

class Controller{
private:
public:
    virtual void chat() {};
    virtual void int_handler() {};
    virtual ControllerType getType() { return NONECON; };
};

#endif