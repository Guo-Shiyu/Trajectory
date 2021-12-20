#include "../../header/battle/battle.h"
#include "../../header/battle/protocol.h"
#include "../../header/battle/server.h"

std::size_t Protocol::LoginBuilder::req_num_{};
sol::state BattleServer::Config{};