#pragma once
#include "../hv/singleton.h"
#include "../client/iluavm.h"
#include "../hv/json.hpp"

using json = nlohmann::json; 
 
class iGameInfo : public iConfig 
{
public:
    virtual bool is_ready() noexcept = 0;
};

class GameInfo : public iGameInfo 
{
    SINGLETON_DECL(GameInfo)
private:
    json room_; // room descriptor 

public:
    GameInfo() : room_() {}
    bool is_ready() noexcept
    {
        // TODO 
        return true;
    }

    GameInfo* ensure() noexcept override 
    {
        return this;
    }
};