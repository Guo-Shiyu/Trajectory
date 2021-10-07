#pragma once

#include "../hv/EventLoopThread.h"

#include "states.h"
#include "ithreads.h"

#include <graphics.h>

using InputFiliter  = std::function<bool(const ExMessage&)>;
using InputReactor  = std::function<void(const ExMessage&)>;
using ExpectedLogic = std::function<void(ExMessage&&)>;

class iUserIO : public WorkThread<StateMachine<iUserIO>, hv::EventLoopThread>
{
    InputFiliter    filiter_;
    InputReactor    reactor_;
    ExpectedLogic   expect_;

public:
    iUserIO()
    {
        this->filiter_ = [](const ExMessage&) { return true; };
        this->reactor_ = [this](const ExMessage& msg) { this->notify(ThreadId::R, "InputLog", ArgsPackBuilder::create(msg)); };
        this->expect_  = [](ExMessage&& msg) {};
    }

    virtual iUserIO *reset_filiter(InputFiliter &&f)  // std::bind(lambda, _1, _2, ...);
    {
        this->eloop_->loop()->pause();
        this->filiter_ = f;
        this->eloop_->loop()->resume();
        return this;
    }

    virtual iUserIO* reset_reactor(InputReactor &&r)
    {
        this->eloop_->loop()->pause();
        this->reactor_ = r;
        this->eloop_->loop()->resume();
        return this;
    }

    virtual iUserIO* reset_expect(ExpectedLogic&& e)
    {
        this->eloop_->loop()->pause();
        this->expect_ = e;
        this->eloop_->loop()->resume();
        return this;
    }

    virtual iUserIO* reset_with(std::optional<InputFiliter> filiter, std::optional<InputReactor> reactor, std::optional<ExpectedLogic> expected) noexcept
    {
        this->eloop_->loop()->pause();
        if (filiter.has_value()) this->filiter_ = filiter.value();
        if (reactor.has_value()) this->reactor_ = reactor.value();
        if (expected.has_value()) this->expect_ = expected.value();
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

    const ExpectedLogic& kexpect() const noexcept
    {
        return this->expect_;
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