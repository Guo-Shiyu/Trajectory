#include "../../header/client/states.h"
#include "../../header/client/client.h"

#define IMPL_STATE(self)   \
    self *self::instance() \
    {                      \
        static self obj;   \
        return &obj;       \
    }

namespace state
{
    namespace client
    {
        IMPL_STATE(Prepare)
        void Prepare::into(Client *c)
        {
            // nothing here
        }

        void Prepare::on(Client *c)
        {
            c->ensure();
            auto net = c->netio()->state();
            auto uio = c->uio()->state();
            clog("check success, net state:{}, userio state:{}",
                 net->get_curent()->as_str(), uio->get_curent()->as_str());

            auto state = c->state();
            if (net->in_state(state::net::Offline::instance()))
                state->into(state::client::Develop::instance());
            else
                state->into(state::client::SignIn::instance());
        }

        void Prepare::off(Client *c)
        {
            c->start();
        }

        IMPL_STATE(SignIn)
        void SignIn::into(Client *c)
        {
        }

        void SignIn::on(Client *c)
        {
            Sleep(10000);
            c->state()->into(state::client::Wrong::instance());
        }

        void SignIn::off(Client *c)
        {
        }

        IMPL_STATE(PickRoom)
        void PickRoom::into(Client *c)
        {
        }

        void PickRoom::on(Client *c)
        {
        }

        void PickRoom::off(Client *c)
        {
        }

        IMPL_STATE(Battle)
        void Battle::into(Client *c)
        {
        }

        void Battle::on(Client *c)
        {
        }

        void Battle::off(Client *c)
        {
        }

        IMPL_STATE(Satistic)
        void Satistic::into(Client *c)
        {
        }

        void Satistic::on(Client *c)
        {
        }

        void Satistic::off(Client *c)
        {
        }

        IMPL_STATE(Wrong)
        void Wrong::into(Client *c)
        {
            c->panic();
        }

        void Wrong::on(Client *c)
        {
            clog("progress exit");
            Logger::log_dump();
        }

        void Wrong::off(Client *c)
        {
            // nothing here
        }

        IMPL_STATE(Develop)
        void Develop::into(Client *c)
        {
        }

        void Develop::on(Client *c)
        {
        }

        void Develop::off(Client *c)
        {
            // can not put any stmt here because initialize Client logic
        }
    }

    namespace net
    {
        IMPL_STATE(Offline)
        void Offline::into(iNetIO *n)
        {
        }

        void Offline::on(iNetIO *n)
        {
        }

        void Offline::off(iNetIO *n)
        {
        }

        IMPL_STATE(ToLoginServ)
        void ToLoginServ::into(iNetIO *n)
        {
            clog("Nio into state ToLoginServer");
            auto cli = n->connect();
            const auto& config = Client::configer()["Config"]["NetIO"];
            std::string addr = config["LoginServerAddr"];
            int port = config["TargetPort"];
            int connfd = cli->createsocket(port, addr.c_str());
            assert(connfd >= 0);

            cli->onConnection = [n](const hv::SocketChannelPtr& channel) {
                std::string peeraddr = channel->peeraddr();
                if (channel->isConnected()) 
                {
                    auto info = std::format("connected to {}, connfd:{}\n", peeraddr.c_str(), channel->fd());
                    channel->write("hello");
                    clog(info);
                    n->notify(ThreadId::R, "NetLog", ArgsPackBuilder::create(std::move(info)));
                }
                else
                {
                    auto info = std::format("disconnected to {}, connfd:{}\n", peeraddr.c_str(), channel->fd());
                    clog(info);
                    n->notify(ThreadId::R, "NetLog", ArgsPackBuilder::create(std::move(info)));
                }
            };

            cli->onMessage = [n](const hv::SocketChannelPtr& channel, hv::Buffer* buf) {
                //printf("< %.*s\n", (int)buf->size(), (char*)buf->data());
                std::string info{static_cast<char*>(buf->data()), buf->size()};
                n->notify(ThreadId::R, "NetLog", ArgsPackBuilder::create(std::move(info)));
            };

            cli->start(false);
        }

        void ToLoginServ::on(iNetIO *n)
        {
        }

        void ToLoginServ::off(iNetIO *n)
        {
        }

        IMPL_STATE(ToBattleServ)
        void ToBattleServ::into(iNetIO *n)
        {
        }

        void ToBattleServ::on(iNetIO *n)
        {
        }

        void ToBattleServ::off(iNetIO *n)
        {
        }
    }

    namespace uio
    { 
        IMPL_STATE(Both)

        void Both::into(iUserIO *u)
        {
            u->reset_with(
                [](const ExMessage& m) noexcept -> bool
                {   return m.message == WM_CHAR;    },
                std::nullopt,
                std::nullopt
            );
        }

        void Both::on(iUserIO *u)
        {
        }

        void Both::off(iUserIO *u)
        {
        }

        IMPL_STATE(OnlyKey)
        void OnlyKey::into(iUserIO *u)
        {
        }

        void OnlyKey::on(iUserIO *u)
        {
        }

        void OnlyKey::off(iUserIO *u)
        {
        }

        IMPL_STATE(OnlyMouse)
        void OnlyMouse::into(iUserIO *u)
        {
        }

        void OnlyMouse::on(iUserIO *u)
        {
        }

        void OnlyMouse::off(iUserIO *u)
        {
        }
    }
}
