#pragma once

#include "../sol/config.hpp"
#include "../sol/forward.hpp"
#include "../sol/sol.hpp"

#include "../hv/hv.h"
#include "../hv/EventLoopThread.h"
#include "../hv/TcpClient.h"


#include "../json/json.hpp"

#include <Netlistmgr.h>
#include <conio.h>
#include <functional>
#include <atomic>
#include <graphics.h>

#include "interface.h"
#include "states.h"
#include "character.h"

using json = nlohmann::json;
using InputFiliter = std::function<bool(ExMessage&)>;

class WorkThreadBase : public MsgInterface, public InitInterface {};

template<typename S, typename T>
class WorkThread : public WorkThreadBase
{
    public:
    using SelfState = S;
    using SelfThread = T;
    
    protected:
    S*  state_;
    T*  eloop_;

    public:
    WorkThread() : state_(nullptr), eloop_(nullptr) {}
    WorkThread(const WorkThread& ) = delete;
    WorkThread& operator = (const WorkThread& ) = delete;

    virtual void start() {}
};

class NetIO : public WorkThread<StateMachine<NetIO>, std::nullptr_t>
{
    SINGLETON_DECL(NetIO)
    private:
    hv::TcpClient* conn_;

    public:
    static json setting_;
    static bool check_net();

    public:
    NetIO() : WorkThread<SelfState, SelfThread>(), conn_(nullptr) {}
    NetIO* init_self() noexcept override final;
    void check() override final;
    void start() override final;
    void on_msg(Message& m) override final;
    void send_msg_to(Recvable target, Recvable self, Message& m) override final;
};

class UserIO : public WorkThread<StateMachine<UserIO>, hv::EventLoopThread>
{
    SINGLETON_DECL(UserIO)
    private:
    InputFiliter filiter_;

    public:
    static json setting_;
    
    public:
    UserIO() : WorkThread<SelfState, SelfThread>(), filiter_(nullptr) {}
    void set_filiter(const InputFiliter&& f);
    UserIO* init_self() noexcept override final;
    void check() override final;
        void on_msg(Message& m) override final;
    void send_msg_to(Recvable target, Recvable self, Message& m) override final;
    
};

class Render : public WorkThread<std::nullptr_t, std::thread>
{
    SINGLETON_DECL(Render)
    private:
    std::atomic_int64_t frame_; // frame counter
    sol::state*  lua_;
    sol::table*  tasks_;
    std::mutex*  lock_;      // keep tasks_ thread safe
    GameInfo*   info_;
    bool        log_;   // log flag

    public:
    static json setting_;

    public:
    Render() : WorkThread<SelfState, SelfThread>() {}
    Render* init_self() noexcept override final;
    void check() override final;
    void on_msg(Message& m) override final;
    void send_msg_to(Recvable target, Recvable self, Message& m) override final;

};