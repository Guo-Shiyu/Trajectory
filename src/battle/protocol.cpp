#include "../../header/battle/server.h"
#include "../../header/battle/protocol.h"
#include "../../header/hv/hlog.h"

Protocol::NetMsgResponser Protocol::LoginRpcMap =
{
	{
		"Hello",
		{
			{
				"Hello", [](BattleServer* server, const json& packet)
				{
					LOGI("Connect to Login Server, self id: %d", packet["Id"].get<int>());
				}
			},
		}
	},

	{
		"Order",
		{
			{
				"StartGame", [](BattleServer* server, const json& packet)
				{
					LOGI("new game started (from login server): %s", packet.dump().c_str());
					size_t roomid = packet["Room"]["Id"].get<int>();
					server->Battles.emplace(roomid, roomid);
					for (auto player_addr : packet["Player"].get<json::array_t>())
					{
						auto ip_port = player_addr.get<std::string>();
						auto ip = ip_port.erase(ip_port.find_first_of(':'), ip_port.back());
						server->AddrMap.insert_or_assign(ip, roomid);
					}

					// area info 
					auto area_name = packet["Room"]["Area"].get<std::string>();
					if (not server->AreaMap.contains(area_name))
					{
						// request uncached area info 
						json apdx;
						apdx["Name"] = area_name;
						server->ToLoginSev.channel->write(LoginBuilder::make()
							.deal_type("Request")
							.deal_subtype("Area")
							.deal_appendix(apdx)
							.build());
					}

					Battle::EventLoop
						->loop()
						->setTimeout(
							1.5 * 1000, [server, area = std::move(area_name), id = packet["Room"]["Id"].get<int>()](auto _)
					{
						auto& btl = server->Battles.at(id);
						btl.Area = new BattleArea(*server->AreaMap.at(area));
						btl.start(Battle::EventLoop->loop());
					});
				}
			},
		},


	},

	{
		"Request",
		{
			{
				"Area", [](BattleServer* server, const json& packet)
				{
					LOGI("new area info gained (from login server): %s", packet.dump().c_str());
					size_t realw{ packet["Width"].get<size_t>() }, realh{ packet["Height"].get<size_t>() };
					auto stream = packet["Stream"].get<std::string>();
					BattleArea* newone = new BattleArea(realw, realh, BattleArea::extract_from_bitstream(stream));
					server->AreaMap[packet["Name"].get<std::string>()] = newone;
				}
			},
		}
	},
};

Protocol::NetMsgResponser Protocol::ClientRpcMap
{
	{
		"Hello",
		{
			{
				"Hello", [](BattleServer* server, const json& packet)
				{
					LOGI("Connect to Login Server, self id: %d", packet["Id"].get<int>());
				}
			},
		}
	},

	{
		"Order",
		{
			{
				"StartGame", [](BattleServer* server, const json& packet)
				{
					LOGI("game started (from client) : %s", packet.dump().c_str());

					auto area_name = packet["Room"]["Area"].get<std::string>();
					if (not server->AreaMap.contains(area_name))
					{
						// request uncached area info 
						json apdx;
						apdx["Name"] = area_name;
						server->ToLoginSev.channel->write(LoginBuilder::make()
							.deal_type("Request")
							.deal_subtype("Area")
							.deal_appendix(apdx)
							.build());
					}

					auto& btl = server->Battles.at(packet["Room"]["Id"].get<int>());
					btl.Area = new BattleArea(*server->AreaMap.at(area_name));
					btl.start(Battle::EventLoop->loop());
				}
			},
		}
	},

};


