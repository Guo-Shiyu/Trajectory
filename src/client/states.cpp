#include "../../header/client/states.h"
#include "../../header/client.h"

#define IMPL_STATE(self)    \
self* self::instance() {    \
    static self obj;        \
    return &obj;            \
}                           \

namespace state {
    namespace client {
        IMPL_STATE(Prepare)
        void Prepare::into(Client* c) 
        {
            // nothing here
        }

        void Prepare::on(Client* c)
        {
            c->check();
            auto net = c->get_netio()->get_state();
            auto uio = c->get_uio()->get_state();
            clog("check success, net state:{}, userio state:{}", 
                net->get_curent()->as_str(), uio->get_curent()->as_str());
            
            auto state = c->get_state();
            if (net->in_state(state::net::Offline::instance()))
                state->into(state::client::Develop::instance());
            else 
                state->into(state::client::SignIn::instance());
        }

        void Prepare::off(Client* c)
        {
            c->start();
        }

        IMPL_STATE(SignIn)
        void SignIn::into(Client* c)
        {
        }

        void SignIn::on(Client* c)
        {
            Sleep(3000);
            c->get_state()->into(state::client::Wrong::instance());
        }

        void SignIn::off(Client* c)
        {

        }

        IMPL_STATE(PickRoom)
        void PickRoom::into(Client* c)
        {

        }

        void PickRoom::on(Client* c)
        {

        }

        void PickRoom::off(Client* c)
        {

        }

        IMPL_STATE(Battle)
        void Battle::into(Client* c)
        {

        }

        void Battle::on(Client* c)
        {

        }

        void Battle::off(Client* c)
        {
            
        }

        IMPL_STATE(Satistic)
        void Satistic::into(Client* c)
        {

        }

        void Satistic::on(Client* c)
        {

        }

        void Satistic::off(Client* c)
        {
            
        }

        IMPL_STATE(Wrong)
        void Wrong::into(Client* c)
        {
            c->on_crash();
        }

        void Wrong::on(Client* c)
        {

        }

        void Wrong::off(Client* c)
        {
            // nothing here
        }

        IMPL_STATE(Develop)
        void Develop::into(Client* c)
        {

        }

        void Develop::on(Client* c)
        {

        }

        void Develop::off(Client* c)
        {
            // can not put any stmt here because initialize Client logic
        }
    }

    namespace net {
        IMPL_STATE(Offline)
        void Offline::into(NetIO* n)
        {

        }

        void Offline::on(NetIO* n)
        {

        }

        void Offline::off(NetIO* n)
        {
            
        }

        IMPL_STATE(ToLoginServ)
        void ToLoginServ::into(NetIO* n)
        {

        }

        void ToLoginServ::on(NetIO* n)
        {

        }

        void ToLoginServ::off(NetIO* n)
        {
            
        }

        IMPL_STATE(ToBattleServ)
        void ToBattleServ::into(NetIO* n)
        {

        }

        void ToBattleServ::on(NetIO* n)
        {

        }

        void ToBattleServ::off(NetIO* n)
        {
            
        }
    }

    namespace uio {
        IMPL_STATE(Both)

        void Both::into(UserIO* u)
        {

        }

        void Both::on(UserIO* u)
        {

        }

        void Both::off(UserIO* u)
        {

        }

        IMPL_STATE(OnlyKey)
        void OnlyKey::into(UserIO* u)
        {

        }

        void OnlyKey::on(UserIO* u)
        {

        }

        void OnlyKey::off(UserIO* u)
        {
            
        }

        IMPL_STATE(OnlyMouse)
        void OnlyMouse::into(UserIO* u)
        {

        }

        void OnlyMouse::on(UserIO* u)
        {

        }

        void OnlyMouse::off(UserIO* u)
        {
            
        }
    }
}


