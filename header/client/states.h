#pragma once
#include <mutex>
#include "../fsmdef.h"
//#include "../client.h"
#include "msgdispatch.h"

class Client;
class NetIO;
class UserIO;
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
        using NetState = StateBase<NetIO>;
        _MACRO_USE_(
            GEN_STATE_3(NetState, NetIO,
                ToLoginServ, ToBattleServ, Offline
            )
        )
    }


    namespace uio {
        using UioState = StateBase<UserIO>;
        _MACRO_USE_(
            GEN_STATE_3(UioState, UserIO,
                Both, OnlyKey, OnlyMouse
            )
        )
    }
}
