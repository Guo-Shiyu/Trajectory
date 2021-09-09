#include "../../header/client/threads.h"

SINGLETON_IMPL(NetIO)
json NetIO::setting_ {};

NetIO* NetIO::init_self() noexcept
{
    this->state_ = new SelfState(this);
    this->state_->set_current(state::net::Offline::instance());
    this->state_->set_previous(this->state_->get_curent());
    this->conn_ = new hv::TcpClient();
    return this;
}

void NetIO::check()
{
    // check setting
    // check net connetion


}

void NetIO::start() 
{

}