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
                        this->connect()->onMessage = [this](const hv::SocketChannelPtr& channel, hv::Buffer* buf) {
                            std::string info{ static_cast<char*>(buf->data()), buf->size() };
                            this->notify(ThreadId::R, "NetLog", ArgsPackBuilder::create(info));
                        };
                        //const auto& config = configer()["Config"]["NetIO"];
                        //json req = json::parse(config["Protocal"]["RequestRooms"].get<std::string>());
                        //req["Appendix"]["ReqCounter"] = req["Appendix"]["ReqCounter"].get<int>() + 1;
                        this->connect()->channel->write(req.dump());
                        // jiang xie yi chongxiang cheng dandu de ming ming kong jian 
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

json& Protocal::instance() noexcept
{
    static json j;
    static std::once_flag flag{};
    std::call_once(flag, [&js = j]() 
        {
            const auto& config = iConfig::configer()["Config"]["NetIO"]["Protocal"];
            auto load_protoccal = [&](auto idx) {    js[idx] = json::parse(config[idx].get<std::string>()); };
            load_protoccal("RequestRooms");
            load_protoccal("Hello");
            load_protoccal("Beat");
            //load_protoccal("RequestRooms");
        });
    return j;
}

std::string Protocal::take(std::string_view idx) noexcept
{
    static std::unordered_map<std::string_view, size_t> counter;
    if (counter.contains(idx))
        counter[idx]++;
    else
        counter[idx] = 1;
    
    if (idx == "RequestRooms")

}

