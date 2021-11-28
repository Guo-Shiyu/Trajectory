#include "../../header/sol/config.hpp"
#include "../../header/sol/forward.hpp"
#include "../../header/sol/sol.hpp"


#include "../../header/hv/TcpClient.h"
#include "../../header/hv/TcpServer.h"

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
    
public:
    BattleServer() = default;

    BattleServer* start() noexcept;

    BattleServer* say_hi() noexcept;


    static void init(std::string&& path) noexcept;
};
