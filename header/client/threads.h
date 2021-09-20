#pragma once

#undef max  // to deal a compile error in sol2
#include "../sol/config.hpp"
#include "../sol/forward.hpp"
#include "../sol/sol.hpp"

#include "../hv/hv.h"
#include "../hv/htime.h"
#include "../hv/EventLoopThread.h"
#include "../hv/TcpClient.h"
#include "../hv/requests.h"

#include "../json/json.hpp"

#include <conio.h>
#include <fstream>
#include <algorithm>
#include <functional>
#include <chrono>
#include <random>
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
    void start() noexcept override final;
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
    void start() noexcept override final;
    void check() override final;
    void on_msg(const Recvable sender, Message&& m) override final;
    void send_msg_to(const Recvable target, const Recvable self, Message&& m) override final;
    
};

class Render : public WorkThread<std::nullptr_t, std::thread>
{
    SINGLETON_DECL(Render)
    private:
    std::atomic_uint64_t frame_; // frame counter
    sol::state* rendee_;
    GameInfo*   info_;
    bool        log_;       // log flag

    public:
    static json setting_;

    public:
    Render() : WorkThread<SelfState, SelfThread>(), rendee_(nullptr), log_(false), info_(nullptr) {}
    decltype(frame_)* cur_frame() noexcept { return &this->frame_; }
    bool* get_logflag() noexcept { return &this->log_; }
    GameInfo* get_ginfo() const noexcept { return this->info_;  }
    sol::state* get_rendee() const noexcept { return this->rendee_;  }

    void set_ginfo(GameInfo* info) noexcept { this->info_ = info; }

    Render* init_self() noexcept override final;
    void check() override final;
    void start() noexcept override final;
    void stop() noexcept;
    void on_msg(const Recvable sender, Message&& m) override final;
    void send_msg_to(const Recvable target, const Recvable self, Message&& m) override final;
    
    // bind Act / Task / Paint with lua state
    void init_state(sol::state* state) noexcept;

    // submit task to render-task queue
    template<typename ...Args>
    void submit_task(const std::string& index, const Args& ...arg) noexcept;

    // submit instant log
    void submit_log(const Recvable self, const std::string& info) noexcept;

    // draw a frame
    bool draw() const noexcept;

    // clear render-task queue
    void clear_task() noexcept;


    // regist render-Act to lua state
    static void regist_act_to(sol::state* state) noexcept;

    // load render-task and object
    static void load_resource_to(sol::state* state);

};

