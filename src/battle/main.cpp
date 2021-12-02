#include "../../header/battle/battle.h"

#include <filesystem>

int main()
{
    //// regist notify func to luavm 
    //std::map<Token, hv::SocketChannelPtr> conn_map;
    //lua.set_function("NotifyTo", [&conn_map](const std::string& addr, const std::string& msg)
    //    {
    //        conn_map.at(addr)->write(msg);
    //    });

    // init battle server with configuration path
    BattleServer::init(std::filesystem::current_path().parent_path().concat("\\sevcfg.lua").string());
    BattleServer sev;
    sev.start();

    LOGI("Server Start Successfully...");
    while (true)
        hv_delay(2);

    // end main
    return 0;
}