#include "../../header/client/render.h"

SINGLETON_IMPL(Render)

Render* Render::lazy_init() noexcept
{
	// sketcher and cache has same action
	this->skter_ = new Sketcher();
	this->cache_ = new Cache();
	this->skter_->initvm()
		->regist_action()->luavm()
		->create_named_table("Schedule", "Submit", [this](std::string_view idx) { this->cacher()->submit(idx); });
	this->cache_->initvm()
		->regist_action()->luavm()
		->create_named_table("Schedule", "Submit", [this](std::string_view idx) { this->cacher()->submit(idx); });

	// bind funtion with lua state and show login cartoon
	std::string path = this->configer()["Config"]["Client"]["ResourcePath"];
	std::filesystem::path resources{ path };
	resources.concat("\\paint"); // ../Trajectory/resource
	load_all_mod(this->skter_->luavm(), resources);
	load_all_mod(this->cache_->luavm(), resources);
	return this;
}

Render* Render::ensure() noexcept
{
	this->configer()["Ensure"]["RenderEnsure"].call();
	this->sktcher()->ensure();
	return this;
}

Render* Render::start() noexcept
{
	this->cacher()->submit("OpenAnimation");
	this->eloop_ = new std::thread(
		[this]()
		{
			const auto& screen = this->configer()["Config"]["Render"]["MainScreen"];
			int width = screen["Width"], hight = screen["Hight"];
			// int cx = GetSystemMetrics(SM_CXSCREEN),  cy = GetSystemMetrics(SM_CYSCREEN);
			int fps = screen["Fps"];
			int expect = 1000 / fps;
			initgraph(width, hight, SHOWCONSOLE);
			while (true)
			{
				auto srt = std::chrono::steady_clock::now();
				this->sktcher()->draw_frame();

				// check new animation
				if (auto ani = this->cacher()->animations(); ani.has_value())
					this->sktcher()->update(ani.value());

				// check new log
				if (auto log = this->cacher()->logs(); log.has_value())
					this->sktcher()->upload(log.value());

				// draw back round 
				if (this->backrd() != nullptr)
					backrd()->display_by(std::nullopt, focusx(), focusy());

				// increase frame counter 
				frame_++;

				// time the frame 
				auto cost = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - srt).count();
				if (frame_ % 20 == 0)
				{
					// log fps every 0.2s(default) 
					auto realfps = cost < 10 ? fps : 1000 / cost;
					this->cacher()->refresh(ThreadId::C, std::to_string(realfps));
				}
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

	this->sktcher()->stop(); //->clear_object()->clear_task();
	this->cacher()->clear();

	closegraph();
	return this;
}