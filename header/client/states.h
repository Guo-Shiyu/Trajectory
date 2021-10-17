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
            GEN_STATE_7(GlobalState, Client, 
                Prepare, SignIn, PickRoom, Battle, Satistic, Wrong, Develop
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
            GEN_STATE_3(UioState, iUserIO,
                SignIn, OnlyKey, OnlyMouse
            )
        )
    }
}
