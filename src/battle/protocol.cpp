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
					LOGI("new game started :");
				}
			},
		}
	},
};

Protocol::NetMsgResponser Protocol::ClientRpcMap
{

};


