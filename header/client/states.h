#pragma once
#include "client.h"



namespace state {
    namespace client {
        using GlobalState = Client::SelfState;
        _MACRO_USE_(
            GEN_STATE_5(GlobalState, Client, 
                Prepare, SignIn, PickRoom, Battle, Satistic, Wrong
            )
        )
    }

    namespace net {
        using NetState = NetIO::SelfState;
        _MACRO_USE_(
            GEN_STATE_2(NetState, NetIO,
                ToLoginServ, ToBattleServ
            )
        )
    }

    namespace uio {
        using UioState = UserIO::SelfState;
        _MACRO_USE_(
            GEN_STATE_3(UioState, UserIO,
                Both, OnlyKey, OnlyMouse
            )
        )
    }
}
