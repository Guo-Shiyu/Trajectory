#include "../../header/client/client.h"

#include <iostream>
#include <random>


int main() 

{
	char str[MAX_PATH] = { 0 };
	get_run_dir(str, sizeof(str));
	std::cout << str << std::endl;
	Client::prepare_for_light();
	auto light = Client::i_say_there_would_be_light();
	light -> shine();
	
	//sol::state lua;
	//lua.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::math, sol::lib::package);
	return 0;
}