#include "../../header/client/client.h"
#include "../../header/client/userio.h"
#include "../../header/client/netio.h"
#include "../../header/client/sprite.h"

// public 
std::mutex iRenderAssembly::GlobalDeviceLock{};

// private
sol::state iConfig::configer_{};
sol::state iRenderAssembly::vm_ {};

std::queue<std::string> Logger::cache_{};

size_t Protocol::LoginBuilder::req_num_{ 0 };

UserIO::KeyMap UserIO::SignInMap = 
{

	
	{	// esc -> exit game
		(char)27, []() 
		{	
			// FIXME: just for test 
			Client::instance()->State->into(state::client::Wrong::instance()); 
		}
	},

	// TODO: tab -> edit self infomation   
	// {}
};

UserIO::KeyMap UserIO::PickRoomMap = 
{
	// c -> create a room 
	{
		'c', []()
		{
			// room : json as str 
			// todo: regist inputbox to luavm ? 
			// std::string rooms =  Client::instance()->configer()["Cmd"]["NewRoom"].call();
			
			// TEST DATA 
			json rd; // room descriptor
			rd["Name"] = "MyRoom";
			rd["Area"] = "Default";

			Dispatcher::dispatch(ThreadId::N, "CreateRoom", ArgsPackBuilder::create(std::move(rd)));
		}},

	// j -> join a room
	{
		'j', []() 
		{	
			// todo: invalid input check 
			wchar_t buf[32];
			InputBox(buf, 10, L"room id: ");

			// convert user input to number 
			int id = _wtoi(buf);
			Dispatcher::dispatch(ThreadId::N, "JoinRoom", ArgsPackBuilder::create(std::move(id)));
		}}
};


UserIO::KeyMap UserIO::InBattleMap = 
{

};

