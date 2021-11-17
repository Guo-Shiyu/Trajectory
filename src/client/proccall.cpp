#include "../../header/client/ilpc.h"
#include "../../header/client/netio.h"
#include "../../header/client/userio.h"
#include "../../header/client/render.h"
#include "../../header/client/client.h"

std::unordered_map<ThreadId, CallMap> Dispatcher::LpcMap =
{
	// empty now 
	{ThreadId::C, {}},

	// 
	{ThreadId::N,
		{
			{
				"SayHello", [](std::optional<ArgsPack> pack)
				{
					assert(NetIO::instance()->state()->in_state(state::net::ToLoginServ::instance()));
					static json hloapdx{};	// appendix of 'Hello' json  
					static std::once_flag flag{};

					std::call_once(flag, []()
						{
							auto& self = hloapdx;
							const sol::table& cfg = Client::configer()["Config"]["Client"]["SelfDescriptor"];
							self["Name"] = cfg["Name"].get<std::string>();
		
							// self["xxxx"] = cfg["xxxxx"]
							// impl in future ... 
						});

					NetIO::instance()
							->connect()
							->channel
							->write(Protocol::LoginBuilder::make()
									.deal_type("Hello")
									.deal_subtype("Hello")
									.deal_appendix(hloapdx)
									.build());
				}
			},

			{
				"RequestRooms", [](std::optional<ArgsPack> pack)
				{
					assert(NetIO::instance()->state()->in_state(state::net::ToLoginServ::instance()));
					auto net = NetIO::instance();
					net->connect()
						->channel
						->write(Protocol::LoginBuilder::make()
								.deal_type("Request")
								.deal_subtype("Room")
								.build());
				}
			},

			{
				"CreateRoom", [](std::optional<ArgsPack> pack)
				{
					assert(NetIO::instance()->state()->in_state(state::net::ToLoginServ::instance()));
					
					json roomdesctiptor = std::any_cast<json>(pack.value()->args_pack().front());

					auto net = NetIO::instance();
					net->connect()
						->channel
						->write(Protocol::LoginBuilder::make()
								.deal_type("Order")
								.deal_subtype("CreateRoom")
								.deal_appendix(roomdesctiptor)
								.build());
				}
			}
	}},

	{ThreadId::R,
		{
			{
				"Debug", [](std::optional<ArgsPack> pack)
				{
					auto arg = std::any_cast<std::string>(pack.value()->args_pack().front());
					Render::instance()
						->cacher()
						->submit("DebugShow", std::move(arg));
				}
			},

			{
				"InputLog", [](std::optional<ArgsPack> pack)
				{
					auto exmsg = std::any_cast<ExMessage>(pack.value()->args_pack().front());
					Render::instance()
						->cacher()
						->refresh(ThreadId::U, as_str(exmsg.vkcode));
				}
			},

			{
				"NetLog", [](std::optional<ArgsPack> pack)
				{
					auto log = std::any_cast<std::string>(pack.value()->args_pack().front());
					Render::instance()
						->cacher()
						->refresh(ThreadId::N, std::move(log));
				}
			},

			{
				"OfflineAlert", [](std::optional<ArgsPack> pack)
				{
					Render::instance()
						->cacher()
						->refresh(ThreadId::N, "Can not connect to server, please check internet connection");
				}
			},
			
			{
				"DisplayRoomList", [](std::optional<ArgsPack> pack)
				{
					// roomlist: json 
					auto roomlist = std::any_cast<json>(pack.value()->args_pack().front());
					
					std::string str = roomlist.dump();
					std::cout << "Request Room List Result: " << str << std::endl;
					
					// Render::instance()
						// ->surface()
						// ->stick("some-name-in-lua", std::move(roomlist))
				}
			},

			{
				"DisplayCreateResult", [](std::optional<ArgsPack> pack)
				{
					// result: json as str 
					auto result = std::any_cast<std::string>(pack.value()->args_pack().front());
					std::cout << "Create Room result: " << result << std::endl;
				}
			},

			{
				"SetBackRound", [](std::optional<ArgsPack> pack)
				{
					// roomlist: json as str 
					// auto roomlist = std::any_cast<BackRound>(pack.value()->args_pack().front());
					// Render::instance()
						// ->surface()
						// ->stick("some-name-in-lua", std::move(roomlist))
				}
			},

	}},

	{ThreadId::U, {}}
};

Protocol::NetMsgResponser Protocol::LoginRpcMap =
{
	{
		"Hello",
		{
			{
				"Hello", [](const json& packet)
				{
					// set self uid field in config
					Client::configer()["Config"]["Client"]["SelfDescriptor"]["Uid"].set(packet["Uid"].get<int>());
				}
			},
		}
	},

	{
		"Reply",
		{
			{
				"Room", [](const json& packet)
				{
					// notify render show room list 
					Dispatcher::dispatch(ThreadId::R, "DisplayRoomList", ArgsPackBuilder::create(packet["Appendix"]));
				}
			},
		}
	},

	{
		"Return",
		{
			{
				"CreateRoom", [](const json& packet)
				{
					// notify render show room list 
					Dispatcher::dispatch(ThreadId::R, "DisplayCreateResult", ArgsPackBuilder::create(packet["Appendix"].dump()));
		}
	},
}
}
};

Protocol::NetMsgResponser Protocol::BattleRpcMap = {};