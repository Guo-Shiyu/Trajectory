#include "../../header/client/render.h"
#include "../../header/client/game.h"

SINGLETON_IMPL(Render)

Render* Render::lazy_init() noexcept
{
	// load scripts for renderer 
	std::string path = this->configer()["Config"]["Client"]["ResourcePath"];
	std::filesystem::path resources{ path };
	resources.concat("\\paint"); // ../Trajectory/resource
	clog("Start Load Resource For Renderer:");
	load_all_mod(&iRenderAssembly::vm_, resources);

	// init logger in screen 
	const auto& screen = this->configer()["Config"]["Render"]["MainScreen"];
	int width = screen["Width"], height = screen["Hight"];
	if (screen["DevloperConsole"].get_or(true))	// default open instant log 
	{
		this->cli_ = new DevelopLogger(0, 0, "FPS:{}");
		this->net_ = new DevelopLogger(0, height - 20, "net:{}");
		this->user_ = new DevelopLogger(width - 100, 0, "key:{}");
	}

	this->Scene = Scene::instance();
	return this;
}

Render* Render::ensure() noexcept
{
	this->renderer()["Ensure"]["RenderEnsure"].call();
	return this;
}

void Render::show_devlog() noexcept
{
	static auto show = [](DevelopLogger* logger)
	{
		if (logger != nullptr)
			logger->cast();
	};

	show(this->cli_);
	show(this->net_);
	show(this->user_);
}

Render* Render::start() noexcept
{
	this->eloop_ = new std::thread(
		[this]()
		{
			// load config file 
			const auto& screen = this->configer()["Config"]["Render"]["MainScreen"];
			int width = screen["Width"], hight = screen["Hight"], fps = screen["Fps"];
			// int cx = GetSystemMetrics(SM_CXSCREEN),  cy = GetSystemMetrics(SM_CYSCREEN);

			// expected cost time of rendering 1 frame  
			int expect = 1000 / fps;

			// TODO: show console decided by config file 
			initgraph(width, hight, SHOWCONSOLE);
			BeginBatchDraw();

			// render loop 
			while (true)
			{
				cleardevice();
				auto srt = std::chrono::steady_clock::now();
				// TODO:recorrect foucus point 

				// ----------------------------------------------- render begin
				

				this->Scene // assert(this->Scene != nullptr ), which been inited in 'lazy_init' proc
					->update(Frame, GameInfo::instance())
					->draw_frame();

				this->show_devlog();

				FlushBatchDraw();
				// ----------------------------------------------- render end 

				// increase frame counter 
				Frame++;

				
				// timing the frame 
				auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - srt).count();
				
				// flush fps flag every 0.2s(default) 
				if (Frame % 5 == 0)
				{
					auto realfps = cost < 10 ? fps : 1000 / cost;
					this->refresh(ThreadId::C, std::format("{:4}, {:4}", realfps, Frame));
				}
				
				// sleep to control FPS 
				if (auto diff = expect - cost; diff > 0)
					Sleep(diff);
				else
					continue;
			}
		});
	this->eloop_->detach();
	return this;
}

Render* Render::panic() noexcept
{
	this->clear_all();
	closegraph();
	return this;
}

Render* Render::clear(RenderLayer layer) noexcept
{
	this->Scene->clear(layer);
	return this;
}

Render* Render::clear_all() noexcept
{
	this->Scene->clear();
	return this;
}

Render* Render::foucus(std::optional<int> x, std::optional<int> y) noexcept
{
	// TODO: 

	return nullptr;
}

Render* Render::refresh(ThreadId id, DevLogBuf&& newlog) noexcept
{
	switch (id)
	{
	case ThreadId::C:
		this->cli_->update(newlog);
		break;
	case ThreadId::R:
		// unreachable code 
		break;
	case ThreadId::N:
		auto now = datetime_now();
		this->net_->update(std::format("[{0:2}:{1:2}:{2:2}] . {3}", now.hour, now.min, now.sec, newlog));
		break;
	case ThreadId::U:
		this->user_->update(newlog);
		break;

	default:
		// unreachable code 
		break;
	}
	return this;
}
