#include "../../header/client/client.h"

std::unordered_map<ThreadId, CallMap> Dispatcher::LpcMap =
{
	//
	{
		ThreadId::C, 
		{
			{
				"RegistRoomInfo", [](std::optional<ArgsPack> pack)
				{
					// field depend on server reply 
					json selfrd = std::any_cast<json>(pack.value()->args_pack().front());
					
					auto str = selfrd.dump();
			std::cout << std::format("RegistRoomInfo called, self's room info:{}", str) << std::endl;
					// save it in configuration
					Client::configer().create_named_table("RoomInfo");
					Client::configer()["RoomInfo"] = Client::configer()["Json"]["decode"].call(std::move(str));
					
					// Client::instance()->game_info()->fill_roominfo(std::move(selfrd));
				}
			},
		}
	},

	// 
	{
		ThreadId::N,
		{
			{
				"SayHello", [](std::optional<ArgsPack> pack)
				{
					assert(NetIO::instance()->State->in_state(state::net::ToLoginServ::instance()));
					static json hloapdx{};	// appendix of 'Hello' json  
					static std::once_flag flag{};

					// init selfinfo once 
					std::call_once(flag, []()
						{
							// load self infomation from configuration 
							auto& self = hloapdx;
							const sol::table& cfg = Client::configer()["Config"]["Client"]["SelfDescriptor"];
							self["Name"] = cfg["Name"].get<std::string>();
							self["Weapon"] = cfg["Weapon"].get<int>();
							const auto& style = cfg["Style"].get<sol::table>();
							for (auto& [k, v] : style)
								self[k.as<std::string>()] = v.as<int>();
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
					assert(NetIO::instance()->State->in_state(state::net::ToLoginServ::instance()));
					
					NetIO::instance()
						->connect()
						->channel
						->write(Protocol::LoginBuilder::make()
								.deal_type("Request")
								.deal_subtype("Room")
								.build());
				}
			},

			{
				"ReqSelfRoomInfo", [](std::optional<ArgsPack> pack)
				{
					assert(NetIO::instance()->State->in_state(state::net::ToLoginServ::instance()));

					json apdx{};
					apdx["RoomId"] = Client::configer()["RoomInfo"]["Id"].get<int>();

					NetIO::instance()
						->connect()
						->channel
						->write(Protocol::LoginBuilder::make()
								.deal_type("Request")
								.deal_subtype("SelfRoom")
								.deal_appendix(std::move(apdx))
								.build());
				}
			},

			{
				"CreateRoom", [](std::optional<ArgsPack> pack)
				{
					assert(NetIO::instance()->State->in_state(state::net::ToLoginServ::instance()));
					
					// rd: room desctiptor, require two field: "Name", "Area"
					json rd = std::any_cast<json>(pack.value()->args_pack().front());

					NetIO::instance()
						->connect()
						->channel
						->write(Protocol::LoginBuilder::make()
								.deal_type("Order")
								.deal_subtype("CreateRoom")
								.deal_appendix(rd)
								.build());
				}
			},

			{
				"JoinRoom", [](std::optional<ArgsPack> pack)
				{
					assert(NetIO::instance()->State->in_state(state::net::ToLoginServ::instance()));

					int id = std::any_cast<int>(pack.value()->args_pack().front());
					json apdx; apdx["TargetId"] = id;

					NetIO::instance()
						->connect()
						->channel
						->write(Protocol::LoginBuilder::make()
								.deal_type("Order")
								.deal_subtype("JoinRoom")
								.deal_appendix(apdx)
								.build());
				}
			},

			{
				"ReqResources", [](std::optional<ArgsPack> pack)
				{
					assert(NetIO::instance()->State->in_state(state::net::ToLoginServ::instance()));
					
					auto req = Protocol::LoginBuilder::make().deal_type("Request");
					const auto& pre = Client::configer();
					if (pre["Area"].get_type() == sol::type::nil)
					{
						json apdx;
						apdx["Name"] = pre["RoomInfo"]["Area"].get<std::string>();
						req.deal_subtype("Area").deal_appendix(apdx);
					}
					// TODO: other resources 
					// else if ()

					NetIO::instance()
						->connect()
						->channel
						->write(req.build());
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
						->submit( 
							RenderLayer::Menu,  
							"DebugShow", 
							Sprite::Updator{ [](auto sprite, auto _)
							{
								static size_t counter = 0;
								if (counter++ % 500 == 0) // continue 5s
									sprite->Age = Sprite::Forever;
							} },
							std::move(arg));
				}
			},

			{
				"InputLog", [](std::optional<ArgsPack> pack)
				{
					auto exmsg = std::any_cast<ExMessage>(pack.value()->args_pack().front());
					Render::instance()->refresh(ThreadId::U, as_str(exmsg.vkcode));
				}
			},

			{
				"NetLog", [](std::optional<ArgsPack> pack)
				{
					auto log = std::any_cast<std::string>(pack.value()->args_pack().front());
					Render::instance()->refresh(ThreadId::N, std::move(log));
				}
			},

			{
				"OfflineAlert", [](std::optional<ArgsPack> pack)
				{
					Render::instance()->refresh(ThreadId::N, "Can not connect to server, please check internet connection");
				}
			},
			
			{
				"DisplayRoomList", [](std::optional<ArgsPack> pack)
				{ 
					auto roomlist = std::any_cast<json>(pack.value()->args_pack().front());
					
					int roomcount = roomlist["Count"];
					sol::table rmarray;
					for (const auto& rm : roomlist["RoomList"])
					{
						sol::table tmp;
						// field depends on 'Create Room'
						tmp["Name"] = rm["Name"].get<std::string>();
						tmp["Area"] = rm["Area"].get<std::string>();

						rmarray.add(tmp);
					}

					Render::instance()
						->clear(RenderLayer::Menu)
						->submit(RenderLayer::Menu, "DisplayRoomList", [](auto _, auto __) {}, roomcount, std::move(rmarray))
						->refresh(ThreadId::N, roomlist.dump());
				}
			},

			{
				"DisplaySelfRoom", [](std::optional<ArgsPack> pack)
				{
					auto msg = std::any_cast<json>(pack.value()->args_pack().front());
					auto str = msg.dump();

					Render::instance()
						->clear(RenderLayer::Menu)
						->submit(RenderLayer::Menu, "DisplaySelfRoom", [](auto _, auto __) {}, str)	// TODO: change it as a table (json.decode(str))
						->refresh(ThreadId::N, std::move(str));
				}
			},

			{
				"DisplayCreateResult", [](std::optional<ArgsPack> pack)
				{
					// result: json 
					auto result = std::any_cast<json>(pack.value()->args_pack().front());
					if (not result["Result"].get<bool>()) // create room fail
					{
						std::string reason = result["Reason"].get<std::string>();
						Render::instance()->refresh(ThreadId::N, std::format("Can not create new room, reason: {}", reason));
					}
					std::cout << "Create Room result: " << result.dump() << std::endl;
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
		"Request",
		{
			{
				"Room", [](const json& packet)
				{
					// notify render show room list 
					Dispatcher::dispatch(ThreadId::R, "DisplayRoomList", ArgsPackBuilder::create(packet));
				}
			},

			{
				"SelfRoom", [](const json& packet)
				{
					// notify render show room list 
					Dispatcher::dispatch(ThreadId::R, "DisplaySelfRoom", ArgsPackBuilder::create(packet));
				}
			},
		}
	},

	{
		"Order",
		{
			{
				"CreateRoom", [](const json& packet)
				{	
					if (packet["Result"].get<bool>()) // success 
					{	 
						// notify client to record self room info   
						Dispatcher::dispatch(ThreadId::C, "RegistRoomInfo", ArgsPackBuilder::create(packet["RoomInfo"]));
						
						// change state
						Client::instance()->State->into(state::client::InRoom::instance());
					}
					else
					{
						Dispatcher::dispatch(ThreadId::R, "CreateRoomFailed", ArgsPackBuilder::create(packet["Reason"]));
					}

					// else: stay in pick room state, do nothing   
				}
			},

			{
				"JoinRoom", [](const json& packet)
				{
					const auto& ret = packet["Appendix"];
					Render::instance()->refresh(ThreadId::N, std::format("Join room reply: {}", ret.dump()));
					// notify render show room list 
					//Dispatcher::dispatch(ThreadId::R, "DisplayJoinResult", ArgsPackBuilder::create(ret));
					//
					//if (ret["Result"].get<bool>()) {	// success 
					//	Client::instance()->State->into(state::client::InRoom::instance());
					//}
					// else: stay in pick room state  
				}
			},
		}
	}
};

Protocol::NetMsgResponser Protocol::BattleRpcMap = {};