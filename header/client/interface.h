#pragma once
class InitInterface 
{
    public:
    virtual InitInterface* init_self()      = 0;
    virtual void check()                    = 0;
    virtual ~InitInterface();
};

enum class Recvable : size_t 
{
    C = 0,  // client
    R = 1,  // render
    N = 2,  // net io
    U = 1,  // user io
};

class Message;
class MsgInterface 
{
    public:
    virtual void on_msg(Message& m) = 0;
    virtual void send_msg_to(Recvable target, Recvable self, Message& m) = 0;
};

enum class CheckResult : size_t
{
    NetConnectFail = 0,
};