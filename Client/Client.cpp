#include "../header/client.h"

int main()
{
    std::cout << "Hello World!\n";

	//initgraph(640, 480);	// 创建绘图窗口，大小为 640x480 像素
	//circle(200, 200, 100);	// 画圆，圆心(200, 200)，半径 100
	//_getch();				// 按任意键继续
	//closegraph();			// 关闭绘图窗口
	//_getch();				// 按任意键继续

	sol::state lua;
	int x = 0;
	lua.open_libraries(sol::lib::base, sol::lib::package);
	lua.set_function("beep", [&x] { ++x; });
	lua.script("beep() print(\"hello\")");
	assert(x == 1);

	json j_no_init_list = json::array();
	json j_empty_init_list = json::array({});
	json j_nonempty_init_list = json::array({ 1, 2, 3, 4 });
	json j_list_of_pairs = json::array({ {"one", 1}, {"two", 2} });

	// serialize the JSON arrays
	std::cout << j_no_init_list << '\n';
	std::cout << j_empty_init_list << '\n';
	std::cout << j_nonempty_init_list << '\n';
	std::cout << j_list_of_pairs << '\n';
	

    std::cout << "Hello World!\n";
}