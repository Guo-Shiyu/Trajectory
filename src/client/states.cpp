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
			void Prepare::into(Client* c)
		{
			// nothing here
		}

		void Prepare::on(Client* c)
		{
			// check initialize work and fix invalid data  
			c->ensure();
			c->State->into(state::client::SignIn::instance());
		}

		void Prepare::off(Client* c)
		{
			// start client program 
			c->start();
		}

		IMPL_STATE(SignIn)
			void SignIn::into(Client* c)
		{
			// change keyboard map
			c->Userio->State->into(state::uio::SignIn::instance());

			// show sign in ani 
			c->Render->submit(RenderLayer::Object, "IntoSignIn");
			Sleep(1000 * 3);

			// make key 'space' usable  
			c->Userio
				->Mapper	// space -> pick room
				->insert_or_assign(' ', []() {
					if (NetIO::instance()->State->in_state(state::net::Offline::instance()))
						Dispatcher::dispatch(ThreadId::R, "OfflineAlert", std::nullopt);
					else
						Client::instance()->State->into(state::client::PickRoom::instance());
				});

			// show main menu 
			c->Render->clear(RenderLayer::Object);
			c->Render->submit(RenderLayer::Menu, "MainMenu");
		}

		void SignIn::on(Client* c)
		{
			// TODO: draw some animation in main menu
			// The Sliver Tree [ i want it !!! ]

			// sleep some time to decrease cpu waste in state machine loop of client  
			Sleep(200);
		}

		void SignIn::off(Client* c)
		{
			// report self infomation to loginserver
			Dispatcher::dispatch(ThreadId::N, "SayHello", std::nullopt);

			// flush screen 
			c->Render->clear(RenderLayer::Menu);
		}

		IMPL_STATE(PickRoom)
			void PickRoom::into(Client* c)
		{
			// switch keyboard-map 
			c->Userio->State->into(state::uio::PickRoom::instance());
		}

		void PickRoom::on(Client* c)
		{
			// request room data by polling
			Dispatcher::dispatch(ThreadId::N, "RequestRooms", std::nullopt);
			Sleep(2 * 1000);
		}

		void PickRoom::off(Client* c)
		{
			// flush screen 
			c->Render->clear(RenderLayer::Menu);
		}

		IMPL_STATE(InRoom)
			void InRoom::into(Client* c)
		{
			// self's room info has been recorded in configer()["RoomInfo"] during RPC process 
			
		}

		void InRoom::on(Client* c)
		{
			static size_t counter{0};

			// poll frequency control params
			constexpr int PSRI = 1 * 1000;		// polling self room infomation interval
			constexpr int RRI  = 0.2 * 1000;	// request resources interval
			constexpr int STAGE_NUM = 5;


			switch (counter % (STAGE_NUM + 1))
			{
			case 0:	// poll self room info, interval = PSRI + STAGE_NUM * RRI
				Dispatcher::dispatch(ThreadId::N, "ReqSelfRoomInfo", std::nullopt);
				Sleep(PSRI);
				break;

			case STAGE_NUM:
				if (GameInfo::instance()->is_ready())
				{
					// make 'space' key usable (start game) 
					UserIO::PickRoomMap.insert_or_assign(' ', []()
						{
							// FIXME............. 
							std::cout << "ohhhhh$^@#$%^&*" << std::endl;

							// change state into Battle
							Client::instance()->State->into(state::client::Battle::instance());
						});
				}
				break;

			default:	
				Sleep(RRI);
				
				// request battle resource stagedly, interval = RRI (5/6) | RRI + PSRI (1/6) 
				if (not GameInfo::instance()->is_ready())
					Dispatcher::dispatch(ThreadId::N, "ReqResources", std::nullopt);
				break;
			}
			counter++;
		}

		void InRoom::off(Client* c)
		{
			// remove key 'space' in pickroom map 
			UserIO::PickRoomMap.erase(' ');  

			// FIXME: new keymap in userio 
		}

		IMPL_STATE(Battle)
			void Battle::into(Client* c)
		{
			std::cout << "ohhhhh$^@#$%^&*" << std::endl;
			Sleep(5 * 1000);
			std::terminate();
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
			c->panic();
		}

		void Wrong::on(Client* c)
		{
			clog("progress exit");
			Logger::log_dump();
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

	namespace net
	{
		IMPL_STATE(Offline)
			void Offline::into(iNetIO* n)
		{
			// log and set reconnect every 
			clog("network wrong, client is offline now");
			hv::ReconnectInfo reconn;
			reconn.min_delay = 1000; // 1s 
			reconn.max_delay = 5000; // 5s
			reconn.delay_policy = 1; // wait 1, 2, 3, 4, 5, 5, 5s ...
			n->connect()->setReconnect(&reconn);
		}

		void Offline::on(iNetIO* n)
		{
			if (n->connect()->channel->isConnected())
				n->State->into(state::net::ToLoginServ::instance());

			// offline timer 
			//static size_t counter{ 0 };
			//constexpr size_t sleeptime{ 1000 }, maxoffline{ 15 };   // 15s 

			//Sleep(sleeptime);
			//if (counter++ == maxoffline)
			//{
			//    clog("too long without net, process has terminated. (not an error)");
			//    Client::instance()->State->into(state::client::Wrong::instance());
			//}
		}

		void Offline::off(iNetIO* n)
		{
		}

		IMPL_STATE(ToLoginServ)
			void ToLoginServ::into(iNetIO* n)
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
					n->State->into(state::net::Offline::instance());
				}

#ifdef _DEBUG
				clog(info);
				Dispatcher::dispatch(ThreadId::R, "NetLog", ArgsPackBuilder::create(std::move(info)));
#endif // _DEBUG

			};

			cli->onMessage = [n](const hv::SocketChannelPtr& channel, hv::Buffer* buf) {
				std::string pack{ static_cast<char*>(buf->data()), buf->size() };

#ifdef _DEBUG
				// show net message on screen 
				Dispatcher::dispatch(ThreadId::R, "NetLog", ArgsPackBuilder::create(pack));
#endif // _DEBUG

				if (n->State->in_state(state::net::ToLoginServ::instance()))
					Protocol::response(Protocol::MsgFrom::LoginServer, pack);
				else
					Protocol::response(Protocol::MsgFrom::BattleServer, pack);
			};

			// start net thread without blocking
			cli->start(false);
		}

		void ToLoginServ::on(iNetIO* n)
		{
			// unused fn
		}

		void ToLoginServ::off(iNetIO* n)
		{
			// unused fn
		}

		IMPL_STATE(ToBattleServ)
			void ToBattleServ::into(iNetIO* n)
		{

		}

		void ToBattleServ::on(iNetIO* n)
		{
		}

		void ToBattleServ::off(iNetIO* n)
		{

		}
	}

	namespace uio
	{
		IMPL_STATE(SignIn)

			void SignIn::into(iUserIO* u)
		{
			u->Mapper = &UserIO::SignInMap;
		}

		void SignIn::on(iUserIO* u)
		{
			// unused
		}

		void SignIn::off(iUserIO* u)
		{
		}

		IMPL_STATE(PickRoom)
			void PickRoom::into(iUserIO* u)
		{
			u->Mapper = &UserIO::PickRoomMap;
		}

		void PickRoom::on(iUserIO* u)
		{
		}

		void PickRoom::off(iUserIO* u)
		{
		}

		IMPL_STATE(OnlyMouse)
			void OnlyMouse::into(iUserIO* u)
		{
		}

		void OnlyMouse::on(iUserIO* u)
		{
		}

		void OnlyMouse::off(iUserIO* u)
		{
		}
	}
}
