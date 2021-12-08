#include "../../header/client/states.h"
#include "../../header/client/client.h"
#include "../../header/client/userio.h"
#include "../../header/client/netio.h"
#include "../../header/client/render.h"
#include "../../header/client/game.h"

#define IMPL_STATE(self)	\
self *self::instance()		\
{							\
    static self obj;		\
    return &obj;			\
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

			c->Userio->State->into(state::uio::SignIn::instance());
			c->Net->State->into(state::net::ToLoginServ::instance());
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
			// =============================================================================
			// c->State->into(state::client::Develop::instance());
			
			// show sign in ani
			constexpr size_t ani_continue_sec = 3; 
			c->Render->submit(RenderLayer::Object, "IntoSignIn");
			Sleep(ani_continue_sec * 1000);

			// make key 'space' usable  
			c->Userio
				->Mapper	// space -> pick room
				->insert_or_assign(' ', []() {
					if (NetIO::instance()->State->in_state(state::net::Offline::instance()))
						Dispatcher::dispatch(ThreadId::R, "OfflineAlert");
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
			Dispatcher::dispatch(ThreadId::N, "SayHello");

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
			Dispatcher::dispatch(ThreadId::N, "RequestRooms");
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
			// self's room info has been recorded in configer()["RoomInfo"] and 
			// game core info wiil be inited conpletely during RPC process 

			// TODO::
			// process roominfo and playerinfo info in game core
			// Client::instance()->GameCore->PlayerInfo = GameCore->RoomInfo.asdasdasdasdasd
		}

		void InRoom::on(Client* c)
		{
			static size_t counter{0};

			// poll frequency control params
			constexpr int PSRI = 1 * 1000;		// polling self room infomation interval
			constexpr int RRI  = 0.5 * 1000;	// request resources interval
			constexpr int STAGE_NUM = 5;

			switch (counter % (STAGE_NUM + 1))
			{
			case 0:	// poll self room info, interval = PSRI + STAGE_NUM * RRI
				Dispatcher::dispatch(ThreadId::N, "ReqSelfRoomInfo");
				Sleep(PSRI);
				break;

			case STAGE_NUM:
				if (GameInfo::instance()->is_ready())
				{
					// make 'space' key usable (start game) 
					UserIO::PickRoomMap.insert_or_assign(' ', []()
						{
							const auto& self = Client::configer()["Config"]["Client"]["SelfDescriptor"];
							const auto& room = Client::configer()["RoomInfo"];
							if (self["Uid"].get<int>() == room["Owner"].get<int>())
								Dispatcher::dispatch(ThreadId::N, "StartGame");
							else
								Render::instance()->refresh(ThreadId::N, "You are not the owner of this room, can't start game");
						});
				}
				else
				{
					// request battle resource stagedly, interval = RRI (5/6) | RRI + PSRI (1/6) 
					Sleep(RRI);
					Dispatcher::dispatch(ThreadId::N, "ReqResources");
				}
				break;
			default:	// unreachable code 
				break;
			}
			counter++;
		}

		void InRoom::off(Client* c)
		{
			// remove key 'space' in pickroom map 
			UserIO::PickRoomMap.erase(' ');

			// notify loginserver to start game 
			Dispatcher::dispatch(ThreadId::N, "StartGame");

			// pause userio thread 
			Client::instance()->Userio->pause();
		}

		IMPL_STATE(Battle)
		void Battle::into(Client* c)
		{
			 
			constexpr size_t wait_seconds = 3U;

			// recount 3s to wair server prepared 
			Client::instance()
				->Render
				->clear_all()
				->submit(RenderLayer::Active, "Recounter", [](auto, auto) {}, wait_seconds);
			
			// wait battle server start and animation 
			Sleep((wait_seconds - 1) * 1000);

			// netio redirect to battle server 
			Client::instance()
				->Net
				->State
				->into(state::net::ToBattleServ::instance());

			// restart userio thread 
			Client::instance()
				->Userio
				->resume()
				->State
				->into(state::uio::InBattle::instance());

			Render::instance()->clear(RenderLayer::Active);

			// init game core and show game scene
			auto area = Client::instance()
				->GameCore
				->ensure()
				->AreaInfo;

			Render::instance()
				->Scene
				->new_sprite(
					RenderLayer::Object,
					Sprite(0U, 
						MagicFn{ [area](int foucusx, int foucusy, size_t frame, auto _)
						{
							settextstyle(BattleArea::fontheight, 0, L"Termianl");
							const auto& field = area->rawbits();
							static std::wstring buf{ 512 };
							for (int y = 0; y < area->Row; y++)
							{
								bool contains{ false };
								for (int x = 0; x < area->Col; x++)
								{
									buf.push_back(field.at(y * area->Col + x) ? '0': ' ');
									contains = true;
								}
								if (contains)
									outtextxy(foucusx, y * BattleArea::unitheight + foucusy, buf.c_str());
								buf.clear();
							}
						} },
						Sprite::Updator{ [render = Render::instance()](auto sprite, auto gameinfo) 
						{
							sprite->X = - render->Fx;
							sprite->Y = - render->Fy;
							sprite->Age++;
						} }));


			clog("GameCore Start ...");
		}

		void Battle::on(Client* c)
		{
			// do nothing 
			Sleep(10);
			/*c->Render->Fx++;
			c->Render->Fy++;*/

			// TODO: possible heartbeat here 
		}

		void Battle::off(Client* c)
		{
			// clear AreaInfo, WeaponInfo 
			auto game = GameInfo::instance();
			delete game->AreaInfo;
			delete game->WeaponInfo;
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
			// clear Roominfo PlayerInfo
			auto game = GameInfo::instance();
			delete game->RoomInfo;
			delete game->PlayerInfo;

			c->Net->State->into(state::net::ToLoginServ::instance());
			c->State->into(state::client::PickRoom::instance());
		}

		IMPL_STATE(Wrong)
		void Wrong::into(Client* c)
		{
			c->panic();
		}

		void Wrong::on(Client* c)
		{
			clog("progress exit with unexpected case");
			Logger::log_dump();
		}

		void Wrong::off(Client* c)
		{
			// nothing here
			// program can't recovery from wrong state
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
			n->Conn->setReconnect(&reconn);
		}

		void Offline::on(iNetIO* n)
		{
			if (n->Conn->channel->isConnected())
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

			// lazy init tcp client 
			n->Conn = new hv::TcpClient();
			auto cli = n->Conn;

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
			if (n->Conn)
				n->Conn->channel->close();
		}

		IMPL_STATE(ToBattleServ)
		void ToBattleServ::into(iNetIO* n)
		{
			// battle server addr has stored in Config->RoomInfo and GameInfo->RoomInfo
			auto sevaddr = Client::configer()["RoomInfo"]["SevAddr"].get<std::string>();
			auto port = n->configer()["Config"]["NetIO"]["BattlePort"].get<int>();
			
			// addr:port -> addr 
			auto delimeter = sevaddr.find_first_of(':');
			sevaddr.erase(delimeter, sevaddr.size() - delimeter);
			
			// recorrect tcp client 
			auto cli = n->Conn;

			// reconstruct new connection 
			int connfd = cli->createsocket(port, sevaddr.c_str());

#ifdef _DEBUG
			assert(connfd >= 0);
#endif // _DEBUG

		}

		void ToBattleServ::on(iNetIO* n)
		{
			// do nothing 
		}

		void ToBattleServ::off(iNetIO* n)
		{
			n->Conn->channel->close();
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


namespace Game
{
	IMPL_STATE(OtherRound)
	void OtherRound::into(iGameInfo* g)
	{

	}

	void OtherRound::on(iGameInfo* g)
	{

	}

	void OtherRound::off(iGameInfo* g)
	{

	}


	IMPL_STATE(SelfRound)
	void SelfRound::into(iGameInfo* g)
	{

	}

	void SelfRound::on(iGameInfo* g)
	{

	}

	void SelfRound::off(iGameInfo* g)
	{

	}
}