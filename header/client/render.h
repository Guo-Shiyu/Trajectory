#pragma once

#include "ithreads.h"
#include "iluavm.h"
#include "sketcher.h"

#include <thread>
#include <string_view>

class iRender : public WorkThread<std::nullptr_t, std::thread>, public iLua
{
protected:
    iSketcher *skter_;
    iRender() : WorkThread(), skter_(nullptr) {}

public:
    iSketcher *sktcher() const noexcept { return this->skter_; }
};

class Render : public iRender
{
    SINGLETON_DECL(Render)
private:
    std::atomic_uint64_t frame_; // frame counter

public:
    Render() : iRender(), frame_(0U) {}

    // init interface
    Render *lazy_init() noexcept override final;
    //void    destory() noexcept override final;

    // condig interface
    Render *ensure() noexcept override final;

    // message interface
    void    response(const ThreadId sender, ProcIndex i, std::optional<ArgsPack> args) noexcept override final;
    void    notify(const ThreadId sender, ProcIndex i, std::optional<ArgsPack> args) noexcept override final;

    // work thread interface
    Render *start() noexcept override final;
    Render *panic() noexcept override final;
};