#pragma once

#include "../hv/EventLoopThread.h"

#include "states.h"
#include "ithreads.h"

#include <graphics.h>

using InputFiliter = std::function<bool(ExMessage &)>;

class iUserIO : public WorkThread<StateMachine<iUserIO>, hv::EventLoopThread>
{
    InputFiliter filiter_;

public:
    iUserIO() = default;
    virtual iUserIO *set_filiter(const InputFiliter &f)
    {
        this->filiter_ = f;
        return this;
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