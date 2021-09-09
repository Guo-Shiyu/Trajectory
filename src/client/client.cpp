#include "../../header/client.h"

void Client::prepare_for_light() 
{
    // load config
    char dir[MAX_PATH];
    get_run_dir(dir, sizeof(dir));
    std::string_view str{dir};
    clog("runtime dictionary: {}", str);
    Client::self_["RuntimeDir"] = std::move(str);
    
    // set check flag true 
    Client::self_["Check"] = true;
}

Client* Client::init_self() noexcept
{
    this->stm_ = new StateMachine<Client>(this);
    this->game_ = new GameInfo();
    this->render_ = new Render();
    this->net_  = new NetIO();
    this->uio_ = new UserIO();
    return this;
}

void Client::shine() noexcept
{
    try
    {
        this->init_self()->check();
        this->stm_->into(state::client::Prepare::instance());
        while(not this->stm_->in_state(state::client::Wrong::instance()))
            this->stm_->execute();
    }
    catch(const std::exception& e)
    {
        clog("catch std::exception:{}", e.what());
    }
    catch(const char* reason)   // check fail wiil throw a const char*
    {
        std::string_view r {reason};
        clog("check before running failed, reason: {}", r);
        Client::self_["CheckFailReason"] = std::move(r);
    }
    catch(...)
    {
        clog("catch nonstd exception, perhaps throwed by sol", " ");
    }
    this->stm_->into(state::client::Wrong::instance());
}

Client* Client::i_say_there_would_be_light()
{
    static Client c;
    return &c;
}