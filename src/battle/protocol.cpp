#include "../../header/battle/protocol.h"
#include "../../header/battle/battle.h"

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
};

Protocol::NetMsgResponser Protocol::ClientRpcMap
{

};


