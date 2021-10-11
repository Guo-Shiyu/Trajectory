#include "../../header/client/netio.h"
SINGLETON_IMPL(NetIO)

NetIO *NetIO::lazy_init() noexcept
{
    this->state_ = new SelfState(this);
    this->state_->set_current(state::net::ToLoginServ::instance());
    this->conn_ = new hv::TcpClient();

    CallMapBuilder builder{};
    builder.set_index("OnMessage")
        ->with_proc([this](std::optional<ArgsPack> pack)
            {
                json msg = json::parse(std::move(std::any_cast<std::string>(pack.value()->args_pack().front())));
                configer()["Config"]["Uid"].set(msg["Uid"].get<int>());
            });

    this->set_proccall_map(builder.build());


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
