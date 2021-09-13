#include "../../header/client.h"

json Client::setting_{};
json Client::self_{};

void Client::prepare_for_light() 
{
    // get runtime dir
    char dir[MAX_PATH];
    get_run_dir(dir, sizeof(dir));
    std::string_view str{dir};
    clog("runtime dictionary: {}", str);
    Client::self_["RuntimeDir"] = std::move(str);

    // load JSON config to setting
    std::fstream in {"clicfg.json", std::ios::in};
    assert(in.is_open(), as_str(ErrCondi::ConfigOpenFail));
    const json& cfg = (in >> Client::setting_, Client::setting_);
    Render::setting_ = cfg["Render"];
    NetIO::setting_ = cfg["NetIO"];
    UserIO::setting_ = cfg["UserIO"];

    // set check flag true 
    Client::self_["Check"] = true;
}

Client* Client::init_self() noexcept
{
    this->stm_ = new StateMachine<Client>(this);
    this->game_ = new GameInfo();   // GameInfo init in state battle
    this->render_ = Render::instance();
    this->render_->init_self();
    this->net_  = NetIO::instance();
    this->net_->init_self();
    this->uio_ = UserIO::instance();
    this->uio_->init_self();
    return this;
}

void Client::check()
{
    this->get_render()->check();
    this->get_netio()->check();
    this->get_uio()->check();
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
    catch(std::string_view reason)   // check fail wiil throw a const char* explain
    {
        clog("check before running failed, reason: {}", reason);
        Client::self_["CheckFailReason"] = std::move(reason);
    }
    catch(...)
    {
        clog("catch nonstd exception, perhaps throwed by sol", " ");
    }

    // program goes wrong
    this->stm_->into(state::client::Wrong::instance());
}

Client* Client::i_say_there_would_be_light()
{
    static Client c;
    return &c;
}

void Client::on_msg(const Recvable sender, Message&& m)
{

}

void Client::send_msg_to(const Recvable target, const Recvable self, Message&& m)
{

}


std::queue<std::string> Logger::log_{};