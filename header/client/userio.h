#pragma once

#include "../hv/EventLoopThread.h"

#include "states.h"
#include "ithreads.h"

#include <graphics.h>

using InputFiliter = std::function<bool(const ExMessage &)>;
using InputReactor = std::function<void(ExMessage&)>;

class iUserIO : public WorkThread<StateMachine<iUserIO>, hv::EventLoopThread>
{
    InputFiliter filiter_;
    InputReactor reactor_;

public:
    iUserIO() = default;
    //virtual iUserIO *set_filiter(InputFiliter &&f)  // std::bind(lambda, _1, _2, ...);
    //{
    //    this->eloop_->loop()->pause();
    //    this->eloop_->loop()->resume();
    //    return this;
    //}

    //virtual iUserIO* set_reactor(InputReactor &&r)
    //{
    //    return this;
    //}

    virtual iUserIO* reset_all_with(InputFiliter&& filiter, InputReactor&& reactor) noexcept
    {
        this->eloop_->loop()->pause();
        this->filiter_ = filiter;
        this->reactor_ = reactor;
        this->eloop_->loop()->resume();
        return this;
    }

    const InputFiliter& kfiliter() const noexcept
    {
        return this->filiter_;
    }

    const InputReactor& kreactor() const noexcept
    {
        return this->reactor_;
    }
};

class UserIO : public iUserIO
{
    SINGLETON_DECL(UserIO);

public:
    UserIO()    = default;
    ~UserIO()   = default;

    // init interface
    UserIO *lazy_init() noexcept override final;

    // condig interface
    UserIO *ensure()    noexcept override final;

    // work thread interface
    UserIO *start() noexcept override final;
    UserIO *panic() noexcept override final;
};