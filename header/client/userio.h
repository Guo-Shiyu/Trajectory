#pragma once

#include "../hv/EventLoopThread.h"

#include "states.h"
#include "ithreads.h"

#include <graphics.h>
#include <unordered_map>

using KeyMap = std::unordered_map<char, std::function<void()>>;

class iUserIO : public WorkThread<StateMachine<iUserIO>, hv::EventLoopThread>
{
protected:
    std::function<void(const char)> mapper_;
public:
    iUserIO() : WorkThread(), mapper_() {}
    iUserIO* set_mapper(decltype(mapper_) mapper)
    {
        this->mapper_ = mapper;
        return this;
    }

    const decltype(mapper_)& kmapper() noexcept 
    {
        return this->mapper_;
    }

    virtual iUserIO *pause() noexcept
    {
        this->eloop_->loop()->pause();
        return this;
    }

    // resume eloop and clear exmsg buffer
    virtual iUserIO *resume() noexcept
    {
        flushmessage();
        this->eloop_->loop()->resume();
        return this;
    }
};

class UserIO : public iUserIO
{
    SINGLETON_DECL(UserIO);

public:
    UserIO() = default;
    ~UserIO() = default;

    // init interface
    UserIO *lazy_init() noexcept override final;

    // condig interface
    UserIO *ensure() noexcept override final;

    // work thread interface
    UserIO *start() noexcept override final;
    UserIO *panic() noexcept override final;

    // defined in 'static.cpp' 
    static  KeyMap  SignInMap, PickRoomMap;
};