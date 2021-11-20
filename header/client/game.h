#pragma once
#include "../hv/singleton.h"
#include "../client/iluavm.h"
#include "../hv/json.hpp"

using json = nlohmann::json; 

class GameInfo : iConfig 
{
    SINGLETON_DECL(GameInfo)
private:
    json room_; // room descriptor 

public:
    GameInfo() : room_() {}
    GameInfo*   fill_roominfo(json&& baseinfo, json&& areainfo) 
    {

        // reset 
        room_["Player"] = json::array(); 
    }
    GameInfo* ensure() noexcept 
    {
        return this;
    }
};