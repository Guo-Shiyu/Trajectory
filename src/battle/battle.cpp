#include "../../header/battle/battle.h"
#include "../../header/battle/protocol.h"





BattleServer* BattleServer::start() noexcept
{
    // connect to login server (as client)
    auto& cli = this->ToLoginSev;
    const sol::table& config = this->Config["Config"];
    std::string addr = config["LoginServerAddr"];
    int port = config["ActivePort"];
    int connfd = cli.createsocket(port, addr.c_str());

    cli.onConnection = [this, port](const hv::SocketChannelPtr& channel) {
        std::string peeraddr = channel->peeraddr();
        std::string info;
        if (channel->isConnected())
        {
            info = std::format("connected to {}, connfd:{}\n", peeraddr, channel->fd());
            this->say_hi();
        }
        else
        {
            LOGI("%s", std::format("Can't connect to LoginServer at {}, program has terminated", peeraddr).c_str());
        }
    };

    cli.onMessage = [this](const hv::SocketChannelPtr& channel, hv::Buffer* buf) {
        std::string pack{ static_cast<char*>(buf->data()), buf->size() };
        Protocol::response(Protocol::MsgFrom::LoginServer, pack, this);
    };

    hv::ReconnectInfo info{};
    info.delay_policy = 5;
    info.max_delay = 10;
    cli.setReconnect(&info);

    // start net thread with blocking
    cli.start(true);

    // start self server 
    auto& sev = this->FromClient;
    assert(sev.createsocket(Config["Config"]["ListenPort"].get<int>()) >= 0);
    sev.setMaxConnectionNum(Config["Config"]["MaxConnNum"].get<int>());
    sev.setThreadNum(Config["Config"]["ThreadNum"].get<int>());

    sev.onConnection = [this](const hv::SocketChannelPtr& channel)
    {
        std::string peeraddr = channel->peeraddr();
        if (channel->isConnected())
        {
            LOGI("- Established\t from:%s connfd=%d", peeraddr.c_str(), channel->fd());
            assert(this->AddrMap.contains(peeraddr));
        }
        else
        {
            LOGI("- Disconnected\t where:%s connfd=%d", peeraddr.c_str(), channel->fd());
        }
    };

    sev.onMessage = [this](const hv::SocketChannelPtr& channel, hv::Buffer* buf)
    {
        std::string pack{ static_cast<char*>(buf->data()), buf->size() };
        std::string addr = channel->peeraddr();
        LOGI("- Receive content: %s from: %s", pack.c_str(), addr.c_str());
        // get message from client 
        Protocol::response(Protocol::MsgFrom::Client, pack, this);
    };

    // start service
    sev.start(false);

    // display play data every 60s
    sev.loop()->setInterval(Config["Config"]["LogInterval"].get<int>() * 1000, [this](hv::TimerID id) {
        std::string log{};
        log.append("\n============= routine log =============\n")
            .append(std::format("cur battle:{}", this->Battles.size()))
            .push_back('\n');

        LOGI("%s", log.c_str());
    });

    return this;
}

BattleServer* BattleServer::say_hi() noexcept
{
    this->ToLoginSev.channel
        ->write(Protocol::LoginBuilder::make()
            .deal_type("Hello")
            .deal_subtype("Hello")
            //.deal_appendix()
            .build());
    return this;
}

void BattleServer::init(std::string&& path) noexcept 
{
    std::once_flag flag{};
    std::call_once(flag, [&lua = BattleServer::Config, path = std::move(path)]() {

        lua.open_libraries(sol::lib::base, sol::lib::coroutine,
            sol::lib::math, sol::lib::package,
            sol::lib::string, sol::lib::table,
            sol::lib::os, sol::lib::io);

        // require 'sevcfg.lua' 
        assert(lua.require_file("Config", path).valid());

        // ensure config 
        if (lua["Config"]["StdoutLog"].get<bool>())
        {
            // reset log stream
            logger_set_handler(hv_default_logger(), stdout_logger);
            LOGI("Using stdout log...");
        }
    });
}
