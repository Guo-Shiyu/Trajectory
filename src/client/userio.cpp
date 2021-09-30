#include "../../header/client/userio.h"

SINGLETON_IMPL(UserIO)

UserIO *UserIO::lazy_init() noexcept
{
    this->state_ = new SelfState(this);
    this->state_->set_current(state::uio::Both::instance()); // set input filiter in Client::Prepare::on()
    return this;
}


UserIO *UserIO::ensure() noexcept
{
    return this;
}


void UserIO::response(const ThreadId sender, ProcIndex i, std::optional<ArgsPack> args) noexcept
{
}

void UserIO::notify(const ThreadId sender, ProcIndex i, std::optional<ArgsPack> args) noexcept
{
}

UserIO *UserIO::start() noexcept
{
    return this;
}

UserIO *UserIO::panic() noexcept
{
    return this;
}

