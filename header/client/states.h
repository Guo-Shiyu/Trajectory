#pragma once

#include "../fsmdef.h"

#include <mutex>

class Client;
class iNetIO;
class iUserIO;
namespace state {
    namespace client {
        using GlobalState = StateBase<Client>;
        _MACRO_USE_(
            GEN_STATE_8(GlobalState, Client, 
                Prepare, SignIn, PickRoom, InRoom, Battle, Satistic, Wrong, Develop
            )
        )
    }

    namespace net {
        using NetState = StateBase<iNetIO>;
        _MACRO_USE_(
            GEN_STATE_3(NetState, iNetIO,
                ToLoginServ, ToBattleServ, Offline
            )
        )
    }

    namespace uio {
        using UioState = StateBase<iUserIO>;
        _MACRO_USE_(
            GEN_STATE_4(UioState, iUserIO,
                SignIn, PickRoom, InBattle, GatheringPower 
            )
        )
    }
}
