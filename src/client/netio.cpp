#include "../../header/client/netio.h"
SINGLETON_IMPL(NetIO)

NetIO *NetIO::lazy_init() noexcept
{
    this->State = new SelfState(this);
    this->State->set_current(state::net::ToLoginServ::instance());
    // this->conn_ = new hv::TcpClient();
    return this;
}

NetIO *NetIO::ensure() noexcept
{
    return this;
}

NetIO *NetIO::start() noexcept
{
    this->State->into(state::net::ToLoginServ::instance());
    this->State->execute();
    return this;
}
NetIO *NetIO::panic() noexcept
{
    return this;
}
