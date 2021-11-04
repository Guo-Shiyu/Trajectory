#include "../header/client/client.h"

std::queue<std::string> Logger::cache_{};
sol::state iConfig::configer_{};
std::unordered_map<ThreadId, iMsg*> Dispatcher::map_{};
std::vector<ProcIndex> CallMapBuilder::index_cache_{};
size_t Protocol::LoginBuilder::req_num_{ 0 };

KeyMap UserIO::SignInMap = {
	{'h', []()
		{
			if (Client::instance()->nio()->state()->in_state(state::net::Offline::instance()))
				Dispatcher::instance()->dispatch(ThreadId::R, ThreadId::U, "OfflineAlert", std::nullopt);
			else 
				Dispatcher::instance()->dispatch(ThreadId::N, ThreadId::U, "RequestRooms", std::nullopt);
		}},


	{'y', []() {	Dispatcher::instance()->dispatch(ThreadId::R, ThreadId::U, "Debug", ArgsPackBuilder::create(std::string("Y has been hitted"))); }}
};

