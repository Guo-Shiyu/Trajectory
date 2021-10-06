#pragma once

#include "states.h"
#include "render.h"
#include "userio.h"
#include "netio.h"
#include "game.h"

class Client :  public iLua, 
                virtual public WorkThread<StateMachine<Client>, std::nullptr_t>
{
    SINGLETON_DECL(Client)
    public:
    using SelfState = StateMachine<Client>;

    private:
    GameInfo*   game_;
    iRender*    render_;
    iNetIO*     net_;
    iUserIO*    uio_;

    public:
    Client() :  iLua(), WorkThread(), game_(nullptr), render_(nullptr), 
                net_(nullptr), uio_(nullptr) {}

    SelfState*  state() const noexcept { return this->state_; }
    GameInfo*   game_info() const noexcept { return this->game_; }
    iRender*    render() const noexcept { return this->render_; }
    iNetIO*     netio() const noexcept { return this->net_; }
    iUserIO*    uio() const noexcept { return this->uio_; }
    iNetIO*     nio() const noexcept { return this->net_; }

    // load and dispatch JSON settings to derived-class
    static void prepare_for_light();

    // Client::instance();  singleton pattern
    static Client* i_say_there_would_be_light();

    // run each thread
    void    shine() noexcept;

    // init interface
    Client* lazy_init() noexcept override final;

    // condig interface
    Client* ensure() noexcept override final;

    // work thread interface
    Client* start() noexcept;
    Client* panic() noexcept;
};