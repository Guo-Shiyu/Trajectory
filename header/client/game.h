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
    using IncreUpdator = std::function<void(BattleArea*)>;
    struct Player 
    {
        int X, Y;
        TurnFace Turn;
    };

public:
    json* RoomInfo;
    json* PlayerInfo;
    BattleArea* AreaInfo;
    IMAGE* ScreenCache;
    int Strength;
    Player Me;

    sol::table* WeaponInfo;

public:
    iGameInfo() : RoomInfo(nullptr), PlayerInfo(nullptr), AreaInfo(nullptr), Strength(0), Me(), WeaponInfo(nullptr), ScreenCache(nullptr) {}
    virtual bool is_ready() noexcept = 0;
    virtual bool is_room_owner() noexcept = 0;
    virtual iGameInfo* ensure() noexcept = 0;
    virtual iGameInfo* update_scn_cache(std::optional<IncreUpdator> incr = std::nullopt) noexcept = 0;
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
        return AreaInfo && WeaponInfo && ScreenCache;
    }

    bool is_room_owner() noexcept override 
    {
        const auto& self = GameInfo::configer()["Config"]["Client"]["SelfDescriptor"];
        const auto& room = GameInfo::configer()["RoomInfo"];

        // enable owner of room to start game 
        return self["Uid"].get<int>() == room["Owner"].get<int>();
    }

    GameInfo* ensure() noexcept override 
    {
        State = new SelfState(this);
        State->set_current(Game::OtherRound::instance());
      
        return this;
    }

    GameInfo* update_scn_cache(std::optional<IncreUpdator> incr = std::nullopt) noexcept override
    {
        if (ScreenCache == nullptr)
            ScreenCache = new IMAGE(AreaInfo->RealWid, AreaInfo->RealHei);

        iRenderAssembly::GlobalDeviceLock.lock();
        SetWorkingImage(ScreenCache);
        settextstyle(BattleArea::FontHeight, BattleArea::FontWidth, L"Terminal");
        if (not incr.has_value())   // full update 
        {
            cleardevice();
            for (size_t i = 0; i < AreaInfo->Row; i++)
            {
                for (size_t j = 0; j < AreaInfo->Col; j++)
                {
                    // TODO: optimize here 
                    // i -> row number 
                    // j -> col number
                    if (AreaInfo->at(i, j))
                        outtextxy(j * BattleArea::UnitWidth, i * BattleArea::UnitHeight, rand_hexdigit());
                }
            }
        }
        else  // incremental update 
        {
            incr.value()(AreaInfo);
        }
        SetWorkingImage(nullptr);
        iRenderAssembly::GlobalDeviceLock.unlock();

        return this;
    }
};