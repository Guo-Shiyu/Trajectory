
#define CASE1

#ifdef CASE1

#include "../../header/client/client.h"

#else

#include <string>
#include "../../header/hv/requests.h"

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

	return 0;
}
#endif // CASE1