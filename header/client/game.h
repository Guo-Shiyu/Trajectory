#pragma once
#include "../hv/singleton.h"
#include "../hv/json.hpp"

#include "../fsmdef.h"
#define _CLIENT
#include "../area.hpp"

using json = nlohmann::json;

enum class TurnFace : bool
{
    Left = true,
    Right = false,
};

class iGameInfo : public iConfig 
{
public:
    json* RoomInfo;
    json* PlayerInfo;
    BattleArea* AreaInfo;
    sol::table* WeaponInfo;

public:
    iGameInfo() : RoomInfo(nullptr), PlayerInfo(nullptr), AreaInfo(nullptr), WeaponInfo(nullptr) {}
    virtual bool is_ready() noexcept = 0;
    virtual iGameInfo* ensure() noexcept = 0;
    // virtual iGameInfo* prepare() noexcept = 0;
    // virtual iGameInfo* display() noexcept = 0;
    // virtual iGameInfo* fix_angle(int incr) noexcept = 0;
    // virtual iGameInfo* turn_face(TurnFace turn) noexcept = 0;
};

namespace Game
{

using GameState = StateBase<iGameInfo>;
_MACRO_USE_(
    GEN_STATE_2(GameState, iGameInfo, 
        OtherRound, SelfRound
    )
)

} // namespace Game 

class GameInfo : public iGameInfo
{
    SINGLETON_DECL(GameInfo)
public:
    using SelfState = StateMachine<iGameInfo>;

public:
    SelfState* State;

public:
    GameInfo() : iGameInfo(), State(nullptr)  {}

    bool is_ready() noexcept
    {
        return AreaInfo && WeaponInfo;
    }

    GameInfo* ensure() noexcept override 
    {
        State = new SelfState(this);
        State->set_current(Game::OtherRound::instance());
      
        return this;
    }

   /* GameInfo* display() noexcept override
    {
        
        return this;
    }*/
};