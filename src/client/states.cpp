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
            // check initialize work and fix invalid data 
            c->ensure();

#ifdef _DEBUG
            auto net = c->netio()->state();
            auto uio = c->uio()->state();
            clog("check success, net state:{}, userio state:{}",
                 net->get_curent()->as_str(), uio->get_curent()->as_str());
#endif // _DEBUG

            c->state()->into(state::client::SignIn::instance());
        }

        void Prepare::off(Client *c)
        {
            c->start();
        }

        IMPL_STATE(SignIn)
        void SignIn::into(Client *c)
        {
            // show sign in ani 
            c->render()->cacher()->submit("IntoSignIn");
            Sleep(1000 * 5);

            c->render()->cacher()->submit("MainMenu");
        }

        void SignIn::on(Client *c)
        {
            Sleep(300);
            // TODO: draw some animation in main menu
            // The Sliver Tree 
        }

        void SignIn::off(Client *c)
        {
            // report self infomation 
            Dispatcher::dispatch(ThreadId::N, "SayHello", std::nullopt);

            // flush 
            c->render()->clear();
        }

        IMPL_STATE(PickRoom)
        void PickRoom::into(Client *c)
        {
            Dispatcher::dispatch(ThreadId::N, "RequestRooms", std::nullopt);
            c->uio()->state()->into(state::uio::PickRoom::instance());
        }   

        void PickRoom::on(Client *c)
        {
            // request room data every two second 
            Sleep(2 * 1000);
            Dispatcher::dispatch(ThreadId::N, "RequestRooms", std::nullopt);
        }

        void PickRoom::off(Client *c)
        {

        }

        IMPL_STATE(InRoom)
        void InRoom::into(Client *c)
        {

        
        }

        void InRoom::on(Client* c)
        {

        }

        void InRoom::off(Client* c)
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
            // log and set reconnect every 
            clog("network wrong, client is offline now");
            hv::ReconnectInfo reconn;
            reconn.min_delay = 1000; // 1s 
            reconn.max_delay = 5000; // 5s
            reconn.delay_policy = 1; // wait 1, 2, 3, 4, 5, 5, 5s ...
            n->connect()->setReconnect(&reconn);
        }

        void Offline::on(iNetIO *n)
        {
            if (n->connect()->channel->isConnected())
                n->state()->into(state::net::ToLoginServ::instance());

            // offline timer 
            //static size_t counter{ 0 };
            //constexpr size_t sleeptime{ 1000 }, maxoffline{ 15 };   // 15s 


            //Sleep(sleeptime);
            //if (counter++ == maxoffline)
            //{
            //    clog("too long without net, process has terminated. (not an error)");
            //    Client::instance()->state()->into(state::client::Wrong::instance());
            //}
        }

        void Offline::off(iNetIO *n)
        {
        }

        IMPL_STATE(ToLoginServ)
        void ToLoginServ::into(iNetIO *n)
        {
            clog("Netio into state ToLoginServer");
            
            auto cli = n->connect();
            const auto& config = Client::configer()["Config"]["NetIO"];
            std::string addr = config["LoginServerAddr"];
            int port = config["TargetPort"];
            int connfd = cli->createsocket(port, addr.c_str());

#ifdef _DEBUG
            assert(connfd >= 0);
#endif // _DEBUG

            cli->onConnection = [n](const hv::SocketChannelPtr& channel) {
                std::string peeraddr = channel->peeraddr();
                std::string info;
                if (channel->isConnected())
                    info = std::format("connected to {}, connfd:{}\n", peeraddr.c_str(), channel->fd());
                else
                {
                    info = std::format("disconnected to {}, connfd:{}\n", peeraddr.c_str(), channel->fd());

                    // change state into offline state 
                    n->state()->into(state::net::Offline::instance());
                }
            
#ifdef _DEBUG
                clog(info);
                Dispatcher::dispatch(ThreadId::R, "NetLog", ArgsPackBuilder::create(std::move(info)));
#endif // _DEBUG

            };

            cli->onMessage = [n](const hv::SocketChannelPtr& channel, hv::Buffer* buf) {
                std::string pack{static_cast<char*>(buf->data()), buf->size()};
               
#ifdef _DEBUG
                // show net message on screen 
                Dispatcher::dispatch(ThreadId::R, "NetLog", ArgsPackBuilder::create(pack));
#endif // _DEBUG

                if (n->state()->in_state(state::net::ToLoginServ::instance()))
                    Protocol::response(Protocol::MsgFrom::LoginServer, pack);
                else
                    Protocol::response(Protocol::MsgFrom::BattleServer, pack);
            };

            // start net thread
            cli->start(false);
        }

        void ToLoginServ::on(iNetIO *n)
        {
            // unused fn
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
        IMPL_STATE(SignIn)

        void SignIn::into(iUserIO *u)
        {
            u->set_mapper([&map = UserIO::SignInMap](const char key) noexcept {
                if (map.contains(key))
                    map[key]();
            });
        }

        void SignIn::on(iUserIO *u)
        {
            // unused fn
        }

        void SignIn::off(iUserIO *u)
        {
        }

        IMPL_STATE(PickRoom)
        void PickRoom::into(iUserIO *u)
        {
            u->set_mapper([&map = UserIO::PickRoomMap](const char key) noexcept {
                if (map.contains(key))
                    map[key]();
            });
            Dispatcher::dispatch(ThreadId::N, "SayHello", std::nullopt);
        }

        void PickRoom::on(iUserIO *u)
        {
        }

        void PickRoom::off(iUserIO *u)
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
