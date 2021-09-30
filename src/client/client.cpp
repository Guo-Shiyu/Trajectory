#include "../../header/client/client.h"

std::queue<std::string> Logger::cache_{};
sol::state iConfig::configer_{};
std::unordered_map<ThreadId, iMsg*> Dispatcher::map_{};

void Client::prepare_for_light()
{
    // get runtime dir
    char dir[MAX_PATH];
    get_run_dir(dir, sizeof(dir));
    std::string str{dir};
    clog("runtime dictionary: {}", str);

    // load config file to setting
    std::string cfile = str.append("\\clicfg.lua");
    std::fstream in{cfile, std::ios::in};
    assert(in.is_open(), as_str(ErrCondi::ConfigOpenFail));
    auto result = Client::configer().safe_script_file(cfile);
    assert(result.valid());
    assert(result.get<bool>());
}

void Client::shine() noexcept
{
    this->state_->into(state::client::Prepare::instance());
    try
    {
        while (not this->state_->in_state(state::client::Wrong::instance()))
            this->state_->execute();
    }
    catch (const std::exception &e)
    {
        clog("catch std::exception:{}", e.what());
    }

    // program goes wrong, execute wrong state
    this->state_->execute();
}

Client *Client::lazy_init() noexcept
{
    this->state_ = new StateMachine<Client>(this);
    this->game_ = new GameInfo(); // GameInfo init in state battle
    this->render_ = Render::instance();
    this->render_->lazy_init();
    this->net_ = NetIO::instance();
    this->net_->lazy_init();
    this->uio_ = UserIO::instance();
    this->uio_->lazy_init();
    return this;
}

Client *Client::ensure() noexcept
{
    this->render()->ensure();
    this->netio()->ensure();
    this->uio()->ensure();
    return this;
}

Client *Client::start() noexcept
{
    this->netio()->start();
    this->uio()->start();
    this->render()->start();
    return this;
}

Client *Client::panic() noexcept
{
    this->render()->panic();
    // this->uio()->panic();
    // this->nio()->panic();

    Logger::log_dump();
    return this;
}

Client *Client::i_say_there_would_be_light()
{
    static Client cli{};
    static std::once_flag oc{};
    std::call_once(oc, [&](){ cli.lazy_init(); });
    return &cli;
}