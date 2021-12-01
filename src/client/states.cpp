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
			constexpr size_t ani_continue_sec = 3; 
			c->Render->submit(RenderLayer::Object, "IntoSignIn");
			Sleep(ani_continue_sec * 1000);

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
			Sleep(20);
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
			constexpr size_t poll_interval = 2;
			Dispatcher::dispatch(ThreadId::N, "RequestRooms", std::nullopt);
			Sleep(poll_interval * 1000);
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

			// pause userio thread 
			Client::instance()->Userio->pause();

			// netio redirect to battle server 
			Client::instance()
				->Net
				->State
				->into(state::net::ToBattleServ::instance());
		}

		IMPL_STATE(Battle)
		void Battle::into(Client* c)
		{
			// recount 3s to wair server prepared  
			constexpr size_t wait_seconds = 3U;

			Client::instance()
				->Render
				->clear_all()
				->submit(RenderLayer::Active, "Recounter", [](auto, auto){}, wait_seconds);
			Sleep(wait_seconds * 1000);

			Client::instance()
				->Userio
				->resume()
				->State
				->into(state::uio::GatheringPower::instance());
		}

		void Battle::on(Client* c)
		{
			// do nothing 
			Sleep(100);

			// TODO: possible heartbeat here 
		}

		void Battle::off(Client* c)
		{
			// clear AreaInfo, WeaponInfo 

		}

		IMPL_STATE(Satistic)
		void Satistic::into(Client* c)
		{
			// request statistic infomation 
		}

		void Satistic::on(Client* c)
		{
			// do nothing 
		}

		void Satistic::off(Client* c)
		{
			// clear Roominfo completely 
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
			// do nothing 
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
			// do nothing 
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

			cli->onConnection = [n](const hv::SocketChannelPtr& channel) 
			{
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

			cli->onMessage = [n](const hv::SocketChannelPtr& channel, hv::Buffer* buf) 
			{
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
			// do nothing 
		}

		void ToLoginServ::off(iNetIO* n)
		{
			// do nothing 
		}

		IMPL_STATE(ToBattleServ)
		void ToBattleServ::into(iNetIO* n)
		{
			// battle server addr has stored in Config->RoomInfo and GameInfo->RoomInfo
			auto sevaddr = Client::instance()
										->GameCore
										->RoomInfo["Sevaddr"].get<std::string>();
			auto port = n->configer()["Config"]["NetIO"]["BattlePort"].get<int>();

			auto cli = n->connect();
			cli->stop();
			int connfd = cli->createsocket(port, sevaddr.c_str());

#ifdef _DEBUG
			assert(connfd >= 0);
#endif // _DEBUG

			// cli->onMessage/onConnection/onWriteComplete needn't change 
			// unblocking start 
			cli->start(false);
		}

		void ToBattleServ::on(iNetIO* n)
		{
			// do nothing 
		}

		void ToBattleServ::off(iNetIO* n)
		{
			// do nothing 
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
			// do nothing 
		}

		void SignIn::off(iUserIO* u)
		{
			// do nothing 
		}

		IMPL_STATE(PickRoom)
		void PickRoom::into(iUserIO* u)
		{
			u->Mapper = &UserIO::PickRoomMap;
		}

		void PickRoom::on(iUserIO* u)
		{
			// do nothing 
		}

		void PickRoom::off(iUserIO* u)
		{
			// do nothing 
		}

		IMPL_STATE(InBattle)
		void InBattle::into(iUserIO* u)
		{
			u->Mapper = &UserIO::InBattleMap;
		}

		void InBattle::on(iUserIO* u)
		{
			// do nothing 
		}

		void InBattle::off(iUserIO* u)
		{
			// do nothing  
		}

		IMPL_STATE(GatheringPower)
		void GatheringPower::into(iUserIO* u)
		{
			// perhaps does not need this state 
			// FIXME::
		}

		void GatheringPower::on(iUserIO* u)
		{
		}

		void GatheringPower::off(iUserIO* u)
		{
		}

		
	}
}
