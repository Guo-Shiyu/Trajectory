#include "../../header/client/userio.h"

SINGLETON_IMPL(UserIO)

UserIO *UserIO::lazy_init() noexcept
{
    this->state_ = new SelfState(this);
    this->state_->set_current(state::uio::Both::instance());
    return this;
}

UserIO *UserIO::ensure() noexcept
{
    return this;
}

UserIO *UserIO::start() noexcept
{
    return this;
}

UserIO *UserIO::panic() noexcept
{
    return this;
}

