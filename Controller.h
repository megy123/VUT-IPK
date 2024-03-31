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