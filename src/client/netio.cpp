#include "../../header/client/netio.h"
SINGLETON_IMPL(NetIO)

NetIO *NetIO::lazy_init() noexcept
{
    this->state_ = new SelfState(this);
    this->state_->set_current(state::net::ToLoginServ::instance());
    this->conn_ = new hv::TcpClient();
    return this;
}

NetIO *NetIO::ensure() noexcept
{
    return this;
}

NetIO *NetIO::start() noexcept
{
    this->state_->into(state::net::ToLoginServ::instance());
    this->state_->execute();
    return this;
}
NetIO *NetIO::panic() noexcept
{
    return this;
}
