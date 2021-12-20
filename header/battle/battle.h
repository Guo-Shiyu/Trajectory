#pragma once 
#include "../../header/sol/config.hpp"
#include "../../header/sol/forward.hpp"
#include "../../header/sol/sol.hpp"

#include "../../header/hv/EventLoopThread.h"
#include "../../header/hv/Channel.h"

#include "../../header/fsmdef.h"
#include "../../header/area.hpp"

#include <list>

// ip address
using Token = std::string; 
 
class Battle
{
public:
    using SelfState = StateMachine<Battle>;

public:
    size_t Id;
    SelfState* State;
    BattleArea* Area;
    sol::state Luavm;
    std::list<Token> Order;
    std::map<Token, hv::SocketChannelPtr> ConnectMap; 

private:
    hv::TimerID timerid_;
    hv::EventLoopPtr loop_;
    Token cur_;

public:
    Battle() = default; // unsafe fn 
    Battle(size_t id) 
        : Id(id), State(nullptr), Area(nullptr), Luavm(), Order(), ConnectMap(), timerid_(0), loop_(nullptr), cur_()
    {
        State = new SelfState(this);
    }

    ~Battle()
    {
        loop_->killTimer(timerid_);
    }

public:
    Battle* start(const hv::EventLoopPtr& loop) noexcept;
    
    hv::EventLoopPtr eveloop() noexcept 
    {
        return this->loop_;
    }

    Battle* next_round() noexcept;

public:
    static hv::EventLoopThreadPtr EventLoop;
    static size_t RoundLongingTime;
    static void check_before_start() noexcept;
};

GEN_STATE_2(StateBase<Battle>, Battle, 
    LoopStage, OtherStage)

