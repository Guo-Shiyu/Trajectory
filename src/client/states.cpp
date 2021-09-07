#include "../header/client.h"

namespace state {
    namespace client {
        template class StateDerive<Client::SelfState, 1>;
        void StateDerive<Client::SelfState, 1>::into(Client* c) 
        {

        }
    }
}