
//#define CASE1

#ifdef CASE1

#include "../../header/client/client.h"

#else

#include <string>
#include "../../header/area.hpp"
#include <graphics.h>
#include <chrono>

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
	initgraph(1240, 940);
	settextstyle(BattleArea::fontheight, 0, L"Termianl");
	auto area_ = BattleArea(330U, 220U);
	auto area = &area_;

	for (int y = 0; y < area->Row; y++)
	{
		for (int x = 0; x < area->Col; x++)
		{
			if (y > area->Row / 2)
				area->set_bit(x, y, true);
		}
	}

	std::vector<int> cache{};
	cache.reserve(1000);
	int Frame = 0;
	while (true)
	{
		cleardevice();
		auto srt = std::chrono::steady_clock::now();
		// TODO:recorrect foucus point 

		// ----------------------------------------------- render begin

		static std::wstring buf{ 512 };
		for (int y = 0; y < area->Row; y++)
		{
			bool contains{ false };
			for (int x = 0; x < area->Col; x++)
			{
				buf.push_back(area->at(y, x)  ? '0' : ' ');
				contains = true;
			}
			if (contains)
				outtextxy(0, y * BattleArea::unitheight, buf.c_str());
			buf.clear();
		}


		FlushBatchDraw();
		// ----------------------------------------------- render end 

		// timing the frame 
		auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - srt).count();

		// flush fps flag every 0.2s(default) 
		if (Frame % 5 == 0)
		{
			auto realfps = 1000 / cost;
			cache.push_back(realfps);
		}

		if (Frame == 100)
		{
			break;
		}

		// sleep to control FPS 
		if (auto diff = 10 - cost; diff > 0)
			Sleep(diff);
		else
			continue;
	}


	return 0;
}
#endif // CASE1