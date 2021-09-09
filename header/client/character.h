#pragma once
#include "../hv/singleton.h"

class GameInfo {
    SINGLETON_DECL(GameInfo)
    
    private:
    int x;
     
    public:
    GameInfo();
};