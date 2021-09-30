#include "../../header/client/render.h"

SINGLETON_IMPL(Render)

Render *Render::lazy_init() noexcept
{

    this->skter_ = new Sketcher();
    // bind funtion with lua state and show login cartoon
    this->skter_->initvm()->regist_act();
    
    std::string path = this->configer()["ResourcePath"];
    path = path.append("\\paint\\");
    auto load_exact_file = [lua = this->sktcher()->luavm(), &path](std::string_view filename, const std::string &modname)
    {
        if (not lua->require_file(modname, path.append(filename)).valid())
            clog("fail to require resource file: {}", path);
        else
            path.erase(path.find(filename), path.length());
    };
    load_exact_file("color.lua", "Color"),
    load_exact_file("ani.lua", "Ani"),
    load_exact_file("object.lua", "Object"),
    load_exact_file("rscript.lua", "Render");
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
    //this->submit_task("OpenAnimation", 600, 600, sol::nil, sol::nil, 600, sol::nil);

    this->eloop_ = new std::thread(
        [this]()
        {
            const auto &screen = this->configer()["Render"]["MainScreen"];
            int width = screen["Width"], hight = screen["Hight"];
            int fps = screen["Fps"];
            int expect = 1000 / fps;
            initgraph(width, hight);
            while (true)
            {
                auto srt = std::chrono::steady_clock::now();
                this->sktcher()->draw_frame()->try_upload();

                if (auto diff = expect - std::chrono::duration_cast<std::chrono::milliseconds>
                                        (std::chrono::steady_clock::now() - srt).count(); diff > 0)
                    Sleep(diff);
                else
                    continue;
            }
        });
    this->eloop_->detach();
    return this;
}

//#undef max  // to deal with a complie error
Render *Render::panic() noexcept
{
    this->sktcher()->stop()->clear_cache()->clear_object()->clear_task();
    closegraph();
    return this;
}

// message interface
void Render::response(const ThreadId sender, ProcIndex i, std::optional<ArgsPack> args) noexcept
{
}

void Render::notify(const ThreadId sender, ProcIndex i, std::optional<ArgsPack> args) noexcept
{
}