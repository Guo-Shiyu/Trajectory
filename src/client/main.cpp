
#define CASE1

#ifdef CASE1

#include "../../header/client/client.h"

#else


#endif // CASE1

#ifdef CASE1

using God = Client;
#include "../../header/client/backround.h"

[[nodiscard("hiahia")]]
int main()
{
	
//initgraph(1540, 952, SHOWCONSOLE);
//BeginBatchDraw();
//BackRound back(1540, 952);
//BackRound::Processor testproc = [](BackRound& bits)
//{
//	for (size_t y = bits.row() / 2 + 20; y < bits.row(); y++)
//		for (size_t x = 0; x < bits.colume(); x++)
//			bits.set_bit(y, x, true);
//
//	size_t centerx = 50, centery = 50, r = 30;
//	for (size_t y = 0; y < bits.row() / 2; y++)
//		for (size_t x = 0; x < bits.colume(); x++)
//		{
//			auto offsetbetween = [](auto a, auto b) { return std::llabs(b - a); };
//			size_t xoffset = offsetbetween(x, centerx), yoffset = offsetbetween(y, centery);
//			if (xoffset * xoffset + yoffset * yoffset < r * (r - 1))
//				bits.set_bit(x, y, true);
//		}
//};
//
//back.process_by(testproc);
//back.display_by();
//std::cout << std::endl;
//std::cout << back.zipped_bitstream() << std::endl;
//std::cout << std::endl;
//EndBatchDraw();


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