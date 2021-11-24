#pragma once

#include "states.h"
#include "render.h"
#include "userio.h"
#include "netio.h"
#include "game.h" 

class Client :  public iLua, virtual public WorkThread<StateMachine<Client>, std::nullptr_t>
{
    SINGLETON_DECL(Client)
public:
    using SelfState = StateMachine<Client>;

public:
    GameInfo*   GameCore;
    iRender*    Render;
    iNetIO*     Net;
    iUserIO*    Userio;

public:
    Client() :  iLua(), WorkThread(), GameCore(nullptr), Render(nullptr), 
                Net(nullptr), Userio(nullptr) {}

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