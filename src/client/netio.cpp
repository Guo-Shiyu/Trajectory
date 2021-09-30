#include "../../header/client/netio.h"
SINGLETON_IMPL(NetIO)

NetIO *NetIO::lazy_init() noexcept
{
    this->state_ = new SelfState(this);
    this->state_->set_current(state::net::ToLoginServ::instance());
    this->state_->set_previous(this->state_->get_curent());
    this->conn_ = new hv::TcpClient();
    return this;
}

NetIO *NetIO::ensure() noexcept
{
    return this;
}

void NetIO::response(const ThreadId sender, ProcIndex i, std::optional<ArgsPack> args) noexcept
{
}
void NetIO::notify(const ThreadId sender, ProcIndex i, std::optional<ArgsPack> args) noexcept
{
}

NetIO *NetIO::start() noexcept
{
    return this;
}
NetIO *NetIO::panic() noexcept
{
    return this;
}

// NetIO::start() noexcept
// {
//     // no heart beat
//     //int beat_init = NetIO::setting_["BeatCountInit"];
//     //std::atomic_int* beat = new std::atomic_int(beat_init);
//     //this->conn_->loop()->setInterval(1000,
//     //    [&beat, &channal = this->conn_->channel](hv::TimerID timerid) {
//     //        beat->fetch_sub(1);
//     //        if (*beat == 0) {
//     //
//     //            channal->send(NetIO::setting_["BaseBeat"].dump());
//     //        }

//     //});

//     // reset client.conn_.onMessage, onConnect, onWriteCompelete

// }