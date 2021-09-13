#include "../../header/client.h"

#include <iostream>
#include <random>

void regist_act(sol::state* l)
{
	l->set_function("Circle", circle);
	l->create_named_table("Act",
		"Circle", circle,
		"Rectangle", rectangle
	);
	l->create_named_table("TaskQue");
}

void load_render_task(sol::state* l)
{
	std::string file{ "auto.lua" };
	std::ifstream in{ file, std::ios::in };
	assert(in.is_open());
	in.close();
	auto result = l->safe_script("require(\"auto\")");
	assert(result.valid());
	l->safe_script(R"(
	print(type(TaskQue))
	print(type(Task).."|"..#Task)
	print(type(StartRender))
	print(type(AddRenderTask))
)");
}

void add_render_task(sol::state* l, std::string index, int x, int y, int r)
{
	l->operator[]("AddRenderTask").call(index, x, y, r);
}

int main() 
{
	char str[MAX_PATH] = { 0 };
	get_run_dir(str, sizeof(str));
	std::cout << str << std::endl;
	//Client::prepare_for_light();
	//auto light = Client::i_say_there_would_be_light();
	//light -> shine();
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::coroutine, sol::lib::math, sol::lib::package);
	regist_act(&lua);
	load_render_task(&lua);

	initgraph(1500, 900);
	setbkcolor(BLACK);
	setcolor(WHITE);
	BeginBatchDraw();
	lua["Circle"](200, 300, 100);
	lua["Act"]["Rectangle"](100, 100, 400, 400);
	//auto r = lua["AddRenderTask"]("CircleExplode", 750, 450, 50);
	EndBatchDraw();

	add_render_task(&lua, "CircleExplode", 750, 450, 50);
	add_render_task(&lua, "CircleExplode", 450, 450, 300);
	std::default_random_engine randengin;
	std::uniform_int_distribution<int> xform(100, 1400);
	std::uniform_int_distribution<int> yform(100, 800);
	for (size_t i = 1; i < 1000; i++)
	{
		BeginBatchDraw();
		cleardevice();
		lua["StartRender"]();
		Sleep(5);
		EndBatchDraw();
		if (i % 10 == 0)
			//lua["AddRenderTask"]("CircleExplode", 300, 400, 400);
			add_render_task(&lua, "CircleExplode", xform(randengin), yform(randengin), 10);
		if (i == 500)
		{
			i += 2;
		}

	}
	closegraph();
	_getch();

	return 0;
}