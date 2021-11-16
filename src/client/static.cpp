#include "../../header/client/client.h"

std::queue<std::string> Logger::cache_{};
sol::state iConfig::configer_{};
size_t Protocol::LoginBuilder::req_num_{ 0 };


KeyMap UserIO::SignInMap = {
	// space -> pick room
	{' ', []()
		{
			if (Client::instance()->nio()->state()->in_state(state::net::Offline::instance()))
				Dispatcher::dispatch(ThreadId::R, "OfflineAlert", std::nullopt);
			else
				Client::instance()->state()->into(state::client::PickRoom::instance());
		}},

	// esc -> exit game
	{ (char)27, []() {	Client::instance()->state()->into(state::client::Wrong::instance()); }},

	// tab -> edit self infomation   
	// {}
};

KeyMap UserIO::PickRoomMap = {
	// c -> create a room 
	{
		'c', []()
		{
			// room : json as str 
			// todo: regist inputbox to luavm 
			//std::string rooms =  Client::instance()->configer()["Cmd"]["NewRoom"].call();
			//Dispatcher::dispatch(ThreadId::N, "CreateRoom", ArgsPackBuilder::create(std::move(rooms)));
		}},

	// j -> join a room
	{
		'j', []() 
		{	
			// todo: invalid input check 
			wchar_t s[32];
			InputBox(s, 10, L"");

			// 将用户输入转换为数字
			int r = _wtoi(s);
		}}
};
