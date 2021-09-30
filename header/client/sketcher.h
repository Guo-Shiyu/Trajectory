#pragma once

#include "iluavm.h"
#include "icommunication.h"

#include <graphics.h>
#include <atomic>
#include <mutex>

class iSketcher : public iLua
{
protected:
    std::mutex lock_;
    sol::table *cache_;     // cache_.is_always_lock_free() == true
    std::atomic_bool log_;  // log flag
    std::atomic_bool draw_; // draw flag
    iSketcher() : iLua(), lock_(), cache_(nullptr), log_(false), draw_(true) {}

public:
    iSketcher *safe_upload() noexcept
    {
        std::lock_guard g{this->lock_};
        this->unsafe_upload();
    }

    iSketcher *unsafe_upload() noexcept
    {
        // this->vm_["Render"]["SubmitCache"].call(*cache);
        return this;
    }

    bool try_upload() noexcept
    {
        if (this->lock_.try_lock())
        {
            this->unsafe_upload(), this->lock_.unlock();
            return true;
        }
        else
            return false;
    }

    iSketcher *clear_cache() noexcept
    {
        std::lock_guard g{this->lock_};
        this->cache_->clear();
        return this;
    }

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

    virtual iSketcher *regist_act() noexcept
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
                                     "Solid",   PS_SOLID,
                                     "Dash",    PS_DASH,
                                     "Dot",     PS_DOT,
                                     "DashDot", PS_DASHDOT);

        this->vm_.create_named_table("FillStyle",
                                     "Solid",   BS_SOLID,
                                     "Null",    BS_NULL,
                                     "Hatched", BS_HATCHED);

        this->vm_.create_named_table("Hatch",
                                     "Cube",    HS_CROSS,
                                     "DiagCube", HS_DIAGCROSS);
        return this;
    }

    virtual iSketcher *draw_frame() noexcept
    {
        if (this->draw_)
        {
            cleardevice();
            BeginBatchDraw();
            this->vm_["Render"]["StepRender"].call();
            if (this->log_)
                this->vm_["Render"]["ShowLog"].call();
            EndBatchDraw();
        }
        return this;
    }

    virtual iSketcher *clear_task() noexcept
    {
        this->clear_cache();
        this->vm_["Render"]["ClearTask"].call();
        return this;
    }

    virtual iSketcher *clear_object() noexcept
    {
        this->clear_cache();
        this->vm_["Render"]["ClearObject"].call();
        return this;
    }

    virtual iSketcher *initvm() noexcept
    {
        this->vm_.open_libraries(sol::lib::base, sol::lib::coroutine,
                                 sol::lib::math, sol::lib::package,
                                 sol::lib::table, sol::lib::string);
        
        // create global table
        this->vm_.create_named_table("TaskQue");
        this->vm_.create_named_table("LogCache");
        this->vm_["ThreadId"] = this->vm_.create_table_with(
            "U", ThreadId::U,
            "N", ThreadId::N,
            "C", ThreadId::C);
        return this;
    }

    virtual iSketcher *load_resource(std::string &&src, std::string &&mod) noexcept = 0;
    virtual iSketcher *ensure() = 0;
    virtual iSketcher* submit() noexcept { return this; };

};

class Sketcher : public iSketcher, public iConfig
{
public:
    Sketcher() : iSketcher() {}

    Sketcher *load_resource(std::string &&src, std::string &&mod) noexcept override final;

    template <typename... Args>
    Sketcher *submit(std::string &&index, Args... args) noexcept;

    Sketcher *ensure() noexcept override final;
};