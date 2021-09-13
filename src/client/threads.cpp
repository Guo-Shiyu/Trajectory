#include "../../header/client/threads.h"

SINGLETON_IMPL(NetIO)
json NetIO::setting_ {};

NetIO* NetIO::init_self() noexcept
{
    this->state_ = new SelfState(this);
    this->state_->set_current(state::net::Offline::instance());
    this->state_->set_previous(this->state_->get_curent());
    this->conn_ = new hv::TcpClient();
    return this;
}

void NetIO::check()
{
    // check setting, such as magic number

    // check net connetion
    const json& cfg = NetIO::setting_;
    /*auto resp = requests::get(cfg["Init"]["NetCheckSite"]);*/
    hv::TcpClient* cli = this->conn_;
    std::string target { cfg["TargetHost"] };
    int port = cfg["SelfPort"];
    int connfd = cli->createsocket(port, target.c_str());
    clog("using port:{}, target host(login server):{}, fd:{}", port, target, connfd);
    if (connfd < 0) throw as_str(CheckResult::NetConnectFail);

    cli->onConnection = [](const hv::SocketChannelPtr& channel) {
        std::string peeraddr = channel->peeraddr();
        if (channel->isConnected()) {
            clog("connected to:{}, connfd:{}", peeraddr.c_str(), channel->fd());
        } else {
            clog("disconnected to:{}, connfd:{}", peeraddr.c_str(), channel->fd());
        }
    };

    hv::TimerID timer = cli->loop()->setTimeout(2500, [](hv::TimerID timerid){
        throw as_str(CheckResult::LoginServNoReply);
    });

    cli->onMessage = [timer, client = this->conn_](const hv::SocketChannelPtr& channel, hv::Buffer* buf) {
        client->loop()->killTimer(timer);
        clog("check net reply len:{}, content:{}", (int)buf->size(), (char*)buf->data());
    };
    
    // reconnect: 1,2,4,8,10,10,10...
    hv::ReconnectInfo reconn;
    reconn.min_delay = 1000;
    reconn.max_delay = 10000;
    reconn.delay_policy = 2;
    cli->setReconnect(&reconn);
    cli->start();
    cli->channel->send("567");
}

void NetIO::start() 
{
    // reset client.conn_.onMessage, onConnect, onWriteCompelete
}

void NetIO::on_msg(const Recvable sender, Message&& m) 
{

}

void NetIO::send_msg_to(const Recvable target, const Recvable self, Message&& m)
{
}

SINGLETON_IMPL(UserIO)
json UserIO::setting_ {};

UserIO* UserIO::init_self() noexcept 
{
    this->state_ = new SelfState(this);
    this->state_->set_current(state::uio::Both::instance());    // set input filiter in Client::Prepare::on()
    return this;
}

void UserIO::check()
{

}

void UserIO::on_msg(const Recvable sender, Message&& m)
{

}

void UserIO::send_msg_to(const Recvable target, const Recvable self, Message&& m)
{

}

SINGLETON_IMPL(Render)
json Render::setting_ {};

Render* Render::init_self() noexcept
{
    // init self except gameinfo, which inited by battle state 
    this->frame_.store(0);
    this->lua_ = new sol::state();
    this->tasks_ = new sol::table();    // task queue, question
    this->lock_ = new std::mutex();
    this->log_ = this->setting_["StartWithLog"];

    // show login cartoon
    this->init_state(this->lua_);   // bind funtion with lua state
    
    return this;
}

void Render::check()
{

}

void Render::init_state(sol::state* lua) noexcept
{
    lua->open_libraries(sol::lib::base, sol::lib::coroutine);   // open all libraris
    this->regist_act_to(lua);
}

void Render::regist_act_to(sol::state* l) noexcept
{
    l->set_function("BeginBatchDraw",       BeginBatchDraw);
    //l->set_function("EndBatchDraw",         EndBatchDraw);
    l->set_function("Sleep",                Sleep);
    //l->set_function("FlushBatchDraw",       FlushBatchDraw);
    l->set_function("ClearDevice",          cleardevice);
}

void Render::on_msg(const Recvable sender, Message&& m)
{

}

void Render::send_msg_to(const Recvable target, const Recvable self, Message&& m)
{
    
}