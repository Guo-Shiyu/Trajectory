#include "../../header/battle/protocol.h"
#include "../../header/battle/battle.h"
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
					LOGI("new game started : %s", packet.dump().c_str());
					for (auto player_addr : packet["Player"].get<json::array_t>())
					{
						auto ip_port = player_addr.get<std::string>();
						auto ip = ip_port.erase(ip_port.find_first_of(':'), ip_port.back());
						server->AddrMap.insert_or_assign(ip, packet["Room"]["Id"].get<int>());
					}
				}
			},
		}
	},
};

Protocol::NetMsgResponser Protocol::ClientRpcMap
{

};


