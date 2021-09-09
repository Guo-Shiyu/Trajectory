#include "../header/client.h"
#include "../header/client/init.h"
#include <future>
#include <iostream>
#include <vector>
#include <coroutine>

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

void load_config(sol::state& lua) {
	static char rundir[MAX_PATH] = { 0 };
	static bool loaded_run_dir = false;
	if (!loaded_run_dir) {
		get_run_dir(rundir, sizeof(rundir)),
		loaded_run_dir = true;
	}
	std::string dir{ rundir };
	dir.erase(dir.find_last_of('\\') - 1, dir.back());
	dir += "\\resource\\auto.lua";
	auto result = lua.load_file(dir);
	std::cout << static_cast<int>(result.status()) << std::endl;
}

void init_lstate(sol::state* state) {
	std::cout << "=== passing arguments to scripts ===" << std::endl;

	sol::state lua;
	lua.open_libraries();
	lua.set_function("bezier", polybezier);
	lua.set_function("circle", circle);

	const auto& my_script = R"(
local a,b= ...
print(a,b)
bezier(a, b)
	)";

	sol::load_result fx = lua.load(my_script);
	sol::function a = fx.get<sol::function>();
	std::function<void(POINT*, int)> b = fx.get<decltype(b)>();
	a(1, 2);
	if (!fx.valid()) {
		sol::error err = fx;
		std::cerr << "failed to load string-based script into the program" << err.what() << std::endl;
	}
	int x = 0;
	int* iptr = &x;
	// prints "your arguments here"
	fx("your", "arguments", "here");
	fx(1, 1.1, "s23");
	fx(iptr, 1.1, "s23");
	POINT pts[] = { {150, 200}, {160, 150}, {240, 150}, {250, 100}, {400, 150}, {500, 150}, {400, 300} };
	std::vector<POINT> pt2s = { {3000, 100}, {400, 150}, {500, 150}, {400, 300} };
	POINT pt3s[] = { {100, 200}, {200, 150}, {300, 150}, {400, 500} };
	sol::function other_fx = lua["bezier"];
	std::function<void(POINT*, int)> cpp_bezier = other_fx;


	static auto scr = R"(
local co = coroutine.create(
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
return co
)";
	sol::object co_obj = lua.script(scr);
	std::function<void()> co_func = co_obj.as<decltype(co_func)>();


	initgraph(2000, 1200);
	setlinecolor(DARKGRAY);
	polyline(pts, 7);		// 画灰色的辅助线
	setlinecolor(GREEN);
	fx(pts, 7);
	fx(pt2s.data(), pt2s.size());
	setlinecolor(YELLOW);
	cpp_bezier(pt2s.data(), pt2s.size());
	b(pt3s, 4);

	for (size_t i = 0; i < 9; i++)
	{
		co_func();
		Sleep(50);
	}
	_getch();				// 按任意键退出
	closegraph();
}

int main()
{

	const char* version = hv_compile_version();
	char run_dir[MAX_PATH] = { 0 };
	get_run_dir(run_dir, sizeof(run_dir));

	std::cout << version << std::endl;
	std::cout << run_dir << std::endl;
    std::cout << "Hello World!\n";
	hlogd("{%d}", 123);

	return 0;
}