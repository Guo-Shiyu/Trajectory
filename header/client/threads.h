#pragma once
#include "../client.h"

template<typename S, typename T>
class WorkThread
{
    public:
    using SelfState = S;
    
    private:
    S*  state_;
    T   eloop_;

    private:
    void init_setting(const json&);

    public:
    void start();
};

class NetIO : public WorkThread<StateBase<NetIO>, hv::EventLoopThread>
{
    private:
    hv::TcpClient conn_;

    public:
    static json setting_;

    void init_netio(const json& cfg);
    public:
};

class UserIO : public WorkThread<StateBase<UserIO>, hv::EventLoopThread>
{
    private:
    InputFiliter filiter_;

    public:
    static json setting_;
    
    public:

    private:
    void set_gio_filiter(InputFiliter filiter);
    void init_uio(const json& cfg);
    
};

class Render : public WorkThread<std::nullptr_t, std::thread>
{
    private:
    std::atomic_int64_t frame_; // frame counter
    sol::state  lua_;
    sol::table  tasks_;
    std::mutex  lock_;  // keep tasks_ thread safe
    GameInfo*   info_;

    public:
    static json setting_;

    void init_render(const json& cfg);
    public:
};