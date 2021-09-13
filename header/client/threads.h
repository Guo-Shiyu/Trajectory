#pragma once

#undef max
#include "../sol/config.hpp"
#include "../sol/forward.hpp"
#include "../sol/sol.hpp"

#include "../hv/hv.h"
#include "../hv/htime.h"
#include "../hv/EventLoopThread.h"
#include "../hv/TcpClient.h"


#include "../json/json.hpp"

#include <conio.h>
#include <fstream>
#include <algorithm>
#include <functional>
#include <atomic>
#include <graphics.h>

#include "states.h"
#include "game.h"
#include "../log.h"

using json = nlohmann::json;
using InputFiliter = std::function<bool(ExMessage&&)>;

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
    S* get_state() const noexcept { return this->state_; };

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
    void on_msg(const Recvable sender, Message&& m) override final;
    void send_msg_to(const Recvable target, const Recvable self, Message&& m) override final;
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
    void on_msg(const Recvable sender, Message&& m) override final;
    void send_msg_to(const Recvable target, const Recvable self, Message&& m) override final;
    
};

class Render : public WorkThread<std::nullptr_t, std::thread>
{
    SINGLETON_DECL(Render)
    private:
    std::atomic_int64_t frame_; // frame counter
    sol::state* lua_;
    sol::table* tasks_;
    std::mutex* lock_;      // keep tasks_ thread safe
    GameInfo*   info_;
    bool        log_;       // log flag

    public:
    static json setting_;

    public:
    Render() : WorkThread<SelfState, SelfThread>() {}
    void set_ginfo(GameInfo* info) noexcept { this->info_ = info; };

    Render* init_self() noexcept override final;
    void check() override final;
    void on_msg(const Recvable sender, Message&& m) override final;
    void send_msg_to(const Recvable target, const Recvable self, Message&& m) override final;


    // bind Act / Task / Paint with lua state
    void init_state(sol::state* state) noexcept;

    // regist render-Act to lua state
    void regist_act_to(sol::state* state) noexcept;

};