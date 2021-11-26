#include <iostream>

#include "../../header/sol/config.hpp"
#include "../../header/sol/forward.hpp"
#include "../../header/sol/sol.hpp"

#include "../../header/hv/json.hpp"
#include "../../header/hv/TcpClient.h"
#include "../../header/hv/TcpServer.h"

using nlohmann::json;
using Token = std::string;

class Battle
{
public:
    size_t Id;
    sol::state Luavm;
    std::map<Token, hv::SocketChannelPtr> ConnectMap; 

private:
    Battle() = delete;
    Battle(size_t id) : Id(id), Luavm(), ConnectMap() {}

public:
    static Battle from_json(json&& packet) noexcept
    {
        // TODO 
    }
};

class BattleServer
{
public:
    using BattleId = size_t;

public:
    std::unordered_map<BattleId, Battle> Battles;
    std::unordered_map<Token, BattleId>  AddrMap;
    hv::TcpClient ToLoginSev;
    hv::TcpServer FromClient;

    static sol::state Config;
    static std::unordered_map<std::string, std::function<void(BattleServer*, json&)>> CmdMap;

public:
    BattleServer() = default;
    BattleServer* start()
    {
        // connect to login server (as client)
        auto& cli = this->ToLoginSev;
        const sol::table& config = this->Config["Config"];
        std::string addr = config["LoginServerAddr"];
        int port = config["ActivePort"];
        int connfd = cli.createsocket(port, addr.c_str());

        cli.onConnection = [port](const hv::SocketChannelPtr& channel) {
            std::string peeraddr = channel->peeraddr();
            std::string info;
            if (channel->isConnected())
                info = std::format("connected to {}, connfd:{}\n", peeraddr, channel->fd());
            else
            {
                info = std::format("disconnected to {}, connfd:{}\n", peeraddr, channel->fd());
                LOGI("%s", std::format("Can't connect to LoginServer at {}:{}, program has terminated", peeraddr, port));
                std::terminate();
            }
        };

        cli.onMessage = [&loginsevaddr = addr, this](const hv::SocketChannelPtr& channel, hv::Buffer* buf) {
            std::string pack{ static_cast<char*>(buf->data()), buf->size() };
            std::string peeraddr = channel->peeraddr();
            if (peeraddr == loginsevaddr) 
            {
                // from login server addr  
                json msg = json::parse(pack);
                this->CmdMap.at(msg[].get<std::string>())(this, msg);
            }
            else
            {
                // from client 
                this->Battles.at(AddrMap.at(peeraddr)).response(peeraddr, pack);
            }
            
        };

        // start net thread without blocking
        cli.start(false);

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
            std::string str{ static_cast<char*>(buf->data()), buf->size() };
            std::string addr = channel->peeraddr();
            LOGI("- Receive content: %s from: %s", str.c_str(), addr.c_str());

            // deal all message in lua
           
            channel->write(reply);
            LOGI("- Reply content: %s to: %s", reply.c_str(), addr.c_str());
            
        };

        // start service
        sev.start();

        //// display play data every 60s
        //sev.loop()->setInterval(lua["Config"]["LogInterval"].get<int>() * 1000, [&lua](hv::TimerID id) {
        //    lua["Main"]["RoutineLog"].call();
        //    });

        return this;
    }

    static void init(std::string&& path)
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
};

sol::state BattleServer::Config{};
std::unordered_map<std::string, std::function<void(BattleServer*, json&)>> BattleServer::CmdMap =
{

};

int main()
{
    //// regist notify func to luavm 
    //std::map<Token, hv::SocketChannelPtr> conn_map;
    //lua.set_function("NotifyTo", [&conn_map](const std::string& addr, const std::string& msg)
    //    {
    //        conn_map.at(addr)->write(msg);
    //    });

    // init battle server with configuration path
    BattleServer::init("../srvcfg.lua");
    BattleServer sev;
    sev.start();

    LOGI("Server Start Successfully...");
    while (true)
        hv_delay(2);

    // end main
    return 0;
}