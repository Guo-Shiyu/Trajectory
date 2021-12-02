#include "../../header/hv/hstring.h"
#include "../../header/client/client.h"
#include "../../header/client/states.h"
#include "../../header/client/userio.h"
#include "../../header/client/netio.h"
#include "../../header/client/render.h"
#include "../../header/client/game.h"

void Client::prepare_for_light()
{
    // get runtime dir
    auto cur = std::filesystem::current_path();

    auto now = datetime_now();
    clog("main start at: [{:2}:{:2}:{:2}], runtime directory:{}",
        now.hour, now.min, now.sec, cur.string());

    // load config file to setting
    auto project = cur.parent_path();
    Client::configer().require_file("Config", project.concat("\\clicfg.lua").string());
    project = project.parent_path();

    // load resource file
    clog("Start Load Resource For Configer VM & Renderer VM");
    load_all_mod(&Client::configer(), project.concat("\\resource\\script"));
    load_all_mod(&Render::renderer(), project);
} 

void Client::shine() noexcept
{
    this->State->into(state::client::Prepare::instance());
    try
    {
        // main game loop 
        while (not this->State->in_state(state::client::Wrong::instance()))
        {
            // step each state machine indivaully
            this->State->execute();
            this->Userio->State->execute();
            this->Net->State->execute();
        }
    }
    catch (const std::exception &e)
    {
        clog("catch std::exception:{}", e.what());
        this->State->into(state::client::Wrong::instance());
    }

    // program goes wrong, execute wrong state
    this->State->execute();
}

Client *Client::lazy_init() noexcept
{
    this->State = new StateMachine<Client>(this); 
    this->GameCore = new GameInfo();       // but GameInfo inited in state 'InRoom'

    this->Render = Render::instance();
    this->Net = NetIO::instance();
    this->Userio = UserIO::instance();
    
    this->Render->lazy_init();
    this->Net->lazy_init();
    this->Userio->lazy_init();
    
    this->vm_.open_libraries(sol::lib::base, sol::lib::string, 
                             sol::lib::package, sol::lib::table, 
                             sol::lib::io, sol::lib::math);
    clog("Start Load Resource For Renderer VM");
    auto resource_path = configer()["Config"]["Client"]["ResourcePath"].get<std::string>();
    load_all_mod(&this->vm_, std::filesystem::path{resource_path}.concat("\\script"));
    return this;
}

Client *Client::ensure() noexcept
{
    this->Render->ensure();
    this->Net->ensure();
    this->Userio->ensure();
    return this;
}

Client *Client::start() noexcept
{
    this->Net->start();
    this->Userio->start();
    this->Render->start();
    return this;
}

Client *Client::panic() noexcept
{
    this->Userio->panic();
    this->Net->panic();
    this->Render->panic(); 
    return this;
}

Client* Client::instance()
{
    static Client cli{};
    return &cli;
}

Client *Client::i_say_there_would_be_light()
{
    return Client::instance()->lazy_init();
}