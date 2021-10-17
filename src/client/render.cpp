#include "../../header/client/render.h"

SINGLETON_IMPL(Render)

Render* Render::lazy_init() noexcept
{
	// sketcher and cache has same action
	this->skter_ = new Sketcher();
	this->cache_ = new Cache();
	this->skter_->initvm()->regist_action();
	this->cache_->initvm()->regist_action();

	// bind funtion with lua state and show login cartoon
	std::string path = this->configer()["Config"]["Client"]["ResourcePath"];
	std::filesystem::path resources{ path };
	resources.concat("\\paint"); // ../Trajectory/resource
	load_all_mod(this->skter_->luavm(), resources);
	load_all_mod(this->cache_->luavm(), resources);

	// gen call_map here
	CallMapBuilder builder{};
	builder.set_index("InputLog")
		->with_proc([this](std::optional<ArgsPack> pack)
					{
						this->cacher()->refresh(ThreadId::U,
												as_str(std::any_cast<ExMessage>(pack.value()->args_pack().front()).vkcode));
					})

		->set_index("NetLog")
		->with_proc([this](std::optional<ArgsPack> pack)
					{
						this->cacher()->refresh(ThreadId::N, std::any_cast<std::string>(pack.value()->args_pack().front()));
					}) ;
	
	this->set_proccall_map(builder.build());

	return this;
}

Render* Render::ensure() noexcept
{
	auto result = this->configer()["Ensure"]["RenderEnsure"].call();
	assert(result.valid());
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
			int fps = screen["Fps"];
			int expect = 1000 / fps;
			initgraph(width, hight, SHOWCONSOLE);
			while (true)
			{
				auto srt = std::chrono::steady_clock::now();
				this->sktcher()->draw_frame();

				// check new animation
				if (auto ani = this->cacher()->animations(); ani.has_value())
					this->sktcher()->update(ani.value()), ani.value().clear();

				// check new log
				if (auto log = this->cacher()->logs(); log.has_value())
					this->sktcher()->upload(log.value()), log.value().clear();

				if (auto diff = expect - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - srt).count(); diff > 0)
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

	this->cacher()->clear();
	this->sktcher()->stop(); //->clear_object()->clear_task();

	closegraph();
	return this;
}