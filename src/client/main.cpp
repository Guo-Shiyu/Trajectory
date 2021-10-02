

#define CASE1

#ifdef CASE1

#include "../../header/client/client.h"

#else

#include <iostream>
#include <random>
#include <filesystem>

#endif // CASE1


#ifdef CASE1
int main() 
{
	auto now = datetime_now();
	clog("main start at: [{:2}:{:2}:{:2}], runtime directory:{}", 
			now.hour, now.min, now.sec, std::filesystem::current_path().string());

	Client::prepare_for_light();
	auto light = Client::i_say_there_would_be_light();
	light -> shine();
	
	return 0;
}

#else
int main()
{
	auto cur = std::filesystem::current_path();
	std::cout << cur.string() << std::endl;
	auto rsrc_path = std::filesystem::path{ cur.parent_path().string().append("\\resource") };
	std::filesystem::directory_iterator all{ rsrc_path };
	for (auto& it : all)
	{
		std::cout << it.path().string() << " " << it.path().filename().string() << std::endl;
	}
	return 0;
}

#endif // CASE1
