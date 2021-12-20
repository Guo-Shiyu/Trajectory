#pragma once 
#include "../../header/battle/battle.h"
#include "../../header/hv/TcpServer.h"
#include "../../header/hv/TcpClient.h"

#include <unordered_map> 


class BattleServer
{
public:
    using BattleId = size_t;

public:
    std::unordered_map<BattleId, Battle> Battles;
    std::unordered_map<Token, BattleId>  AddrMap;
    std::unordered_map<std::string, BattleArea*> AreaMap;
    hv::TcpClient ToLoginSev;
    hv::TcpServer FromClient;

    static sol::state Config;

private:
    BattleServer* say_hi() noexcept;
    
public:
    BattleServer() = default;

    BattleServer* start() noexcept;


    static void init(std::string&& path) noexcept;
};
