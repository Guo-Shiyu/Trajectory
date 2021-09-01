#include "../header/client.h"
#include <future>
#include <iostream>

void func(sol::state& lua)
{
	static auto count = 0;
	static auto script = R"(
co = coroutine.create(
function()
local x, y = 200, 200
local start_r = 100
local step = 20
	for i = 1, 10 do
		circle(x, y, start_r)
		start_r = start_r + step
		coroutine.yield()
	end
end
)
)";
	static auto call = R"(
coroutine.resume(co)
return coroutine.status(co)
)";

	if (count >= 0) {
		if (count == 0)
			lua.script(script);

		std::string s = lua.script(call);
		std::cout << s << std::endl;
		
		count++;
	}

}


int main()
{

	const char* version = hv_compile_version();
	char run_dir[MAX_PATH] = { 0 };
	get_run_dir(run_dir, sizeof(run_dir));

	std::cout << version << std::endl;
	std::cout << run_dir << std::endl;
    std::cout << "Hello World!\n";

	sol::state lua;
	int z = 0;
	lua.open_libraries(sol::lib::base, sol::lib::package);
	lua.open_libraries();
	lua.set_function("circle", [](int x, int y, int r) { circle(x, y, r); });
	lua.script("print(type(coroutine))");
	std::cout << lua.memory_used() << std::endl;
	_getch();
	

	json j_no_init_list = json::array();
	json j_empty_init_list = json::array({});
	json j_nonempty_init_list = json::array({ 1, 2, 3, 4 });
	json j_list_of_pairs = json::array({ {"one", 1}, {"two", 2} });

	// serialize the JSON arrays
	std::cout << j_no_init_list << '\n';
	std::cout << j_empty_init_list << '\n';
	std::cout << j_nonempty_init_list << '\n';
	std::cout << j_list_of_pairs << '\n';

	srand((unsigned)time(NULL));

	// 初始化图形模式
	//initgraph(640, 480);

	//int  x, y;
	//char c;

	//settextstyle(16, 8, _T("Courier"));	// 设置字体

	//// 设置颜色
	//settextcolor(GREEN);
	//setlinecolor(BLACK);

	//for (int i = 0; i <= 479; i++)
	//{
	//	// 在随机位置显示三个随机字母
	//	for (int j = 0; j < 3; j++)
	//	{
	//		x = (rand() % 80) * 8;
	//		y = (rand() % 20) * 24;
	//		c = (rand() % 26) + 65;
	//		outtextxy(x, y, c);
	//	}

	//	// 画线擦掉一个像素行
	//	line(0, i, 639, i);

	//	Sleep(10);					// 延时
	//	if (i >= 479)	i = -1;
	//	if (_kbhit())	break;		// 按任意键退出
	//}

	//// 关闭图形模式
	//closegraph();
	return 0;
	

    std::cout << "Hello World!\n";
}