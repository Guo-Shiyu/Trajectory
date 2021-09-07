#include "sol/config.hpp"
#include "sol/forward.hpp"
#include "sol/sol.hpp"

#include "json/json.hpp"
using json = nlohmann::json;

#include "hv/hv.h"
#include "hv/singleton.h"
#include "hv/hstring.h"
#include "hv/EventLoopThread.h"
#include "hv/TcpClient.h"

#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <functional>
using InputFiliter = std::function<bool(const ExMessage&)>;

#include <mutex>
#include <atomic>

#include <easyx.h>

#include "fsmdef.h"
#include "client/states.h"
#include "client/threads.h"
#include "client/character.h"


class Client {
    public:
    using SelfState = StateBase<Client>;

    private:
    SelfState*  state_;
    GameInfo    game_;
    Render      render_;
    NetIO       net_;
    UserIO      uio_;

    public:
    static json setting_;

    public:
    Client();

    // run each thread &&
    void start();

    // load and dispatch JSON settings to derived-class
    static void prepare_for_light();

    // Client::new();  singleton pattern
    static Client* i_say_there_would_be_light();

    private:
    // call each init func of member and initialize self
    void init_client();

    // call each stop func of member and log to screen
    void stop_client();

    // check net / setting / lua script is all right
    bool check_condition();
};