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
    size_t  Id;
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
