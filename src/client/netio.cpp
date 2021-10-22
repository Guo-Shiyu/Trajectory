#include "../../header/client/netio.h"
SINGLETON_IMPL(NetIO)

NetIO *NetIO::lazy_init() noexcept
{
    this->state_ = new SelfState(this);
    this->state_->set_current(state::net::ToLoginServ::instance());
    this->conn_ = new hv::TcpClient();

    CallMapBuilder builder{};
    builder.set_index("OnHello")
        ->with_proc([this](std::optional<ArgsPack> pack)
                    {
                        json msg = json::parse(std::move(std::any_cast<std::string>(pack.value()->args_pack().front())));
                        configer()["Config"]["Uid"].set(msg["Uid"].get<int>());
                    })
        ->set_index("RequestRooms")
        ->with_proc([this](std::optional<ArgsPack> pack)
                    {
                        this->connect()->onMessage = [this](const hv::SocketChannelPtr &channel, hv::Buffer *buf)
                        {
                            std::string info{static_cast<char *>(buf->data()), buf->size()};
                            this->notify(ThreadId::R, "NetLog", ArgsPackBuilder::create(info));
                        };
                        this->connect()->channel->write(Protocol::LoginBuilder::make()
                                                                                .deal_type(ProtoType::Request)
                                                                                .deal_subtype(ReqSubType::Room)
                                                                                .build());
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

//// todo: there may divide into two json
//json& Protocol::instance() noexcept
//{
//    static json proto;
//    static std::once_flag flag{};
//    std::call_once(flag, [&]()
//        {
//            json login, battle;
//            try
//            {
//                const auto& config = iConfig::configer()["Config"]["NetIO"]["Protocol"];
//                login = json::parse(config["ToLoginServer"].get<std::string>());
//                // battle = json::parse(config["ToBattleServer"].get<std::string>());
//            }
//            catch(const std::exception& e)
//            {
//                clog("protocal load failed: {}", e.what());
//                std::terminate();
//            }
//            proto["Login"] = login;
//            proto["Battle"] = battle;
//        });
//
//    return proto;
//}
