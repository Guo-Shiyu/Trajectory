#pragma once

#include "iluavm.h"
#include "icommunication.h"

#include <graphics.h>
#include <atomic>
#include <mutex>

class iRenderAssembly : public iLua
{
protected:
    iRenderAssembly() : iLua(){};
    virtual ~iRenderAssembly(){};

    iRenderAssembly *open_libraries() noexcept
    {
        this->vm_.open_libraries(sol::lib::base, sol::lib::coroutine,
                                 sol::lib::math, sol::lib::package,
                                 sol::lib::table, sol::lib::string);
        return this;
    }

public:
    virtual iRenderAssembly *regist_action() noexcept
    {
        this->vm_.create_named_table(
            "Set",
            "SetLineStyle", [](int style, int thickness = 1)
            { setlinestyle(style, thickness); },

            "SetLineColor", setlinecolor,

            "SetFillStyle", [](int style, long hatch = 0L)
            { setfillstyle(style, hatch); },

            "SetFillColor", setfillcolor,

            "SetTextStyle", [](int height, int width, std::string &&font)
            { settextstyle(height, width,
                           std::wstring(font.begin(), font.end()).c_str()); });

        this->vm_.create_named_table(
            "Act",
            "Xyout", [](int x, int y, std::string &&text)
            { outtextxy(x, y,
                        std::wstring(text.begin(), text.end()).c_str()); },

            "Circle", circle,
            "Line", line);

        this->vm_.create_named_table("LineStyle",
                                     "Solid", PS_SOLID,
                                     "Dash", PS_DASH,
                                     "Dot", PS_DOT,
                                     "DashDot", PS_DASHDOT);

        this->vm_.create_named_table("FillStyle",
                                     "Solid", BS_SOLID,
                                     "Null", BS_NULL,
                                     "Hatched", BS_HATCHED);

        this->vm_.create_named_table("Hatch",
                                     "Cube", HS_CROSS,
                                     "DiagCube", HS_DIAGCROSS);
        this->vm_.create_named_table("ThreadId",
            "C", ThreadId::C,
            "U", ThreadId::U,
            "N", ThreadId::N,
            "R", ThreadId::R);
        return this;
    }

    virtual iRenderAssembly *load_resource(std::string &&src, std::string &&mod) noexcept
    {
        this->luavm()->require_script(mod, src);
        return this;
    }

    virtual iRenderAssembly *initvm() noexcept = 0;
    virtual iRenderAssembly *ensure() = 0;
};

// thread safe cache for sketcher
class iCache : public iRenderAssembly
{
private:
    static constexpr std::string_view TASKS_CACHE   = "TaskCache";
    static constexpr std::string_view LOGS_CACHE    = "LogCache";
    //static constexpr std::string_view SELF_SCRIPT   = "Rscript";

protected:
    std::mutex lock_;
    std::atomic_bool log_modified_;
    std::atomic_bool task_modified_;

    iCache() : lock_(), log_modified_(false), task_modified_(false) {}

public:

    iCache *initvm() noexcept
    {
        this->open_libraries();
        this->vm_.create_named_table(TASKS_CACHE);
        this->vm_.create_named_table(LOGS_CACHE);
        return this;
    }

    // bool has_new_log() const noexcept
    // {
    //     return this->log_modified_;
    // }

    // bool has_new_task() const noexcept 
    // {
    //     return this->task_modified_;
    // }

    bool modified() const noexcept
    {
        return this->log_modified_ || this->task_modified_;
    }

    std::optional<sol::table> animations() noexcept 
    {
        std::lock_guard guard{ this->lock_ };
        std::optional<sol::table> ret{std::nullopt};
        if (this->task_modified_)
        {
            sol::table tab = this->vm_[TASKS_CACHE];
            ret.emplace(std::move(tab)); 
            this->task_modified_ = false;
        }
        return ret;
    }

    std::optional<sol::table> logs() noexcept 
    {
        std::lock_guard guard{ this->lock_ };
        std::optional<sol::table> ret{ std::nullopt };
        if (this->log_modified_) {
            sol::table tab = this->vm_[LOGS_CACHE];
            ret.emplace(std::move(tab));
            this->log_modified_ = false;
        }
        return ret;
    }

    // std::optional<sol::table> provide(ProvideType t) noexcept
    // {
    //     bool flag = (t == ProvideType::Log);
    //     std::optional<sol::table> ret{std::nullopt};
    //     if (flag)
    //         if (this->log_modified_)
    //         {
    //             ret = this->vm_["LogCache"].get<decltype(ret)>();
    //             this->vm_["LogCache"].set(sol::nil);
    //             this->log_modified_ = false;
    //         }
    //         else if (this->task_modified_)
    //         {
    //             ret = this->vm_["TaskQue"].get<sol::table>();
    //             this->vm_["TaskQue"].set(sol::nil);
    //             this->task_modified_ = false;
    //         }

    //     return ret;
    // }

    // std::optional<sol::table> provide(std::string_view key) noexcept
    // {
    //     std::lock_guard guard{this->lock_};
    //     std::optional<sol::table> ret = this->vm_[key];
    //     if (ret.has_value())
    //         this->vm_[key].set(sol::nil);
    //     return ret;
    // }

    // iCache *prepare(std::string_view key)
    // {
    //     std::lock_guard guard{this->lock_};
    //     this->vm_.create_named_table(key);
    //     return this;
    // }

    bool try_own() noexcept
    {
        return this->lock_.try_lock();
    }

    iCache* to_owned() noexcept
    {
        this->lock_.lock();
        return this;
    }

    iCache *release() noexcept
    {
        this->lock_.unlock();
        return this;
    }

    iCache *clear() noexcept
    {
        std::lock_guard guard{this->lock_};
        sol::table table = this->vm_[TASKS_CACHE];
        table.clear();
        this->task_modified_ = false;
        
        table = this->vm_[LOGS_CACHE];
        table.clear();
        this->log_modified_ = false;

        return this;
    }

    template <typename... Args> // submit task to task cache
    iCache *submit(std::string_view index, Args... args) noexcept
    {
        std::lock_guard guard{this->lock_};
        this->vm_["Rscript"]["AddRenderTask"].call(index, args ...);
        this->task_modified_ = true;
        return this;
    }

    iCache *refresh(ThreadId id, std::string &&newlog) noexcept
    {
        std::lock_guard guard{this->lock_};
        this->vm_["Rscript"]["AddRenderLog"].call(id, newlog);
        this->log_modified_ = true;
        return this;
    }
};

class iSketcher : public iRenderAssembly
{
protected:
    std::atomic_bool log_;  // log flag
    std::atomic_bool draw_; // draw flag
    iSketcher() : iRenderAssembly(), log_(true), draw_(true) {}

public:
    iSketcher *display_log() noexcept
    {
        this->log_.store(true);
        return this;
    }

    iSketcher *conceal_log() noexcept
    {
        this->log_.store(false);
        return this;
    }

    iSketcher *start() noexcept
    {
        this->draw_.store(true);
        return this;
    }

    iSketcher *stop() noexcept
    {
        this->draw_.store(false);
        return this;
    }

    virtual iSketcher *draw_frame() noexcept
    {
        if (this->draw_)
        {
            cleardevice();
            BeginBatchDraw();

            // draw log at first 
            if (this->log_)
                this->vm_["Rscript"]["ShowLog"].call();
            
            this->vm_["Rscript"]["StepRender"].call();
            EndBatchDraw();
        }
        return this;
    }

    virtual iSketcher *clear_task() noexcept
    {
        assert(this->vm_["Rscript"]["ClearTask"].call().valid());
        return this;
    }

    virtual iSketcher *clear_object() noexcept
    {
        assert(this->vm_["Rscript"]["ClearObject"].call().valid());
        return this;
    }

    virtual iSketcher *clear_all() noexcept
    {
        this->clear_object();
        this->clear_task();
        return this;
    }

    // upload new log
    virtual iSketcher *upload(sol::table logs) noexcept
    {
        this->vm_["Rscript"]["UploadLog"].call(logs);
        logs.clear();
        return this;
    }

    // update new render task
    virtual iSketcher *update(sol::table tasks) noexcept
    {
        this->vm_["Rscript"]["UpdateTask"].call(tasks);
        tasks.clear();
        return this;
    }
};

class Cache : public iCache
{
public:
    Cache() : iCache() {}
    Cache *ensure() noexcept override final;
};

class Sketcher : public iSketcher, public iConfig
{
public:
    Sketcher() : iSketcher() {}

    Sketcher *initvm() noexcept override final;
    Sketcher *ensure() noexcept override final;
};
