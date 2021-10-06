#include "../../header/client/render.h"

SINGLETON_IMPL(Render)

Render *Render::lazy_init() noexcept
{
    // sketcher and cache has same action
    this->skter_ = new Sketcher();
    this->cache_ = new Cache();
    this->skter_->initvm()->regist_action();    
    this->cache_->initvm()->regist_action();

    // bind funtion with lua state and show login cartoon
    std::string path = this->configer()["Config"]["Client"]["ResourcePath"];
    std::filesystem::path resources{path};
    resources.concat("\\paint"); // ../Trajectory/resource
    load_all_mod(this->skter_->luavm(), resources);
    load_all_mod(this->cache_->luavm(), resources);
    return this;
}

Render *Render::ensure() noexcept
{
    auto result = this->configer()["Ensure"]["RenderEnsure"].call();
    assert(result.valid());
    this->sktcher()->ensure();
    return this;
}

Render *Render::start() noexcept
{
    this->cacher()->submit(SubmitType::Ani, "OpenAnimation", 600, 600, sol::nil, sol::nil, 600, sol::nil);

    this->eloop_ = new std::thread(
        [this]()
        {
            const auto &screen = this->configer()["Config"]["Render"]["MainScreen"];
            int width = screen["Width"], hight = screen["Hight"];
            int fps = screen["Fps"];
            int expect = 1000 / fps;
            initgraph(width, hight, SHOWCONSOLE);
            while (true)
            {
                auto srt = std::chrono::steady_clock::now();
                this->sktcher()->draw_frame();
                if (this->cacher()->modified())
                    if (this->cacher()->try_own()) // to_owned success
                        this->sktcher()->update(this->cacher())->upload(this->cacher()),this->cacher()->release();
                // expect to be like this :
                // if (this->sketcher()->modified())
                // {
                //      ProvideType type;
                //      if (this->cacher()->log_modified()) {
                //           type = ProvideType::log;
                //      else if (this->cacher()->task_modified())
                //           type = ProvideType::task;
                //      this->sketcher->require(type, this->cacher()->provide(type));
                // }

                if (auto diff = expect - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - srt).count(); diff > 0)
                    Sleep(diff);
                else
                    continue;
            }
        });
    this->eloop_->detach();
    return this;
}

Render *Render::panic() noexcept
{
    this->sktcher()->stop()->clear_object()->clear_task();

    this->cacher()->clear_all();

    closegraph();
    return this;
}