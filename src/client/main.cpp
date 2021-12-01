
#define CASE1

#ifdef CASE1

#include "../../header/client/client.h"

#else


#endif // CASE1

#ifdef CASE1
using God = Client;

[[nodiscard("hiahia")]]
int main()
{
	std::cout << "Hello " << std::endl;
	God::prepare_for_light();
	auto light = God::i_say_there_would_be_light();
	return light->shine();
}



#else
int main()
{
	Program prog;
	prog.start()->dump_to_file();

	return 0;
}
#endif // CASE1