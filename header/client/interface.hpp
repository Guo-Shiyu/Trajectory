#pragma once

#include <iterator>
#include <array>
#include <string_view>

class InitInterface 
{
    public:
    virtual InitInterface* init_self()      = 0;
    virtual void check()                    = 0;
    virtual ~InitInterface()                {};
};

enum class Recvable : size_t 
{
    C = 0,  // client
    R = 1,  // render
    N = 2,  // net io
    U = 3,  // user io
};

class Message;
class MsgInterface 
{
    public:
    virtual void on_msg(const Recvable sender, Message&& m) = 0;
    virtual void send_msg_to(const Recvable target, const Recvable self, Message&& m) = 0;
};

// namespace default 
// {
    
// } // namespace default
