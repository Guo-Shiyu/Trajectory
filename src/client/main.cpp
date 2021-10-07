

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
#include "../../header/hv/TcpClient.h"
using namespace hv;

int main() {
    int port = 1234;

    while (1) hv_sleep(1);
    return 0;
}
#endif // CASE1
