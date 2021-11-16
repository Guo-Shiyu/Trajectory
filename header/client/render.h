#pragma once

#include "ithreads.h"
#include "iluavm.h"
#include "sketcher.h"
#include "backround.h"

#include <thread>
#include <string_view>

class iRender : public WorkThread<std::nullptr_t, std::thread>, public iLua
{
protected:
    iSketcher*  skter_;
    iCache*     cache_;
    BackRound*  back_;      // init in state 'pickroom-off'
    int         fcsx_, fcsy_; // scope foucus point 
    iRender() : WorkThread(), skter_(nullptr), cache_(nullptr), back_(nullptr), fcsx_(0), fcsy_(0) {}

public:
    iSketcher*  sktcher() const noexcept { return this->skter_; }
    iCache*     cacher() const noexcept { return this->cache_; }
    iRender*    clear() noexcept { this->cache_->clear(), this->skter_->clear_all(); return this; }
    BackRound*  backrd() const noexcept { return this->back_; }
    iRender*    set_backround(BackRound* backrdptr) noexcept { this->back_ = backrdptr; return this; }
    iRender*    foucus_on(std::optional<int> x = std::nullopt, std::optional<int> y = std::nullopt) noexcept 
    {
        fcsx_ = x.value_or(fcsx_);
        fcsy_ = y.value_or(fcsy_);
        return this;
    }
    int         focusx() const noexcept { return fcsx_; }
    int         focusy() const noexcept { return fcsy_; }
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

    // work thread interface
    Render *start() noexcept override final;
    Render *panic() noexcept override final;
};