#include "../../header/client/threads.h"

SINGLETON_IMPL(NetIO)
json NetIO::setting_ {};

NetIO* NetIO::init_self() noexcept
{
    this->state_ = new SelfState(this);
    this->state_->set_current(state::net::ToLoginServ::instance());
    this->state_->set_previous(this->state_->get_curent());
    this->conn_ = new hv::TcpClient();
    return this;
}

void NetIO::check()
{
    // check net connetion
    const json& cfg = NetIO::setting_;
    hv::TcpClient* cli = this->conn_;
    std::string target { cfg["TargetHost"] };
    
    int port = cfg["SelfPort"];
    int connfd = cli->createsocket(port, target.c_str());
    clog("using port:{}, target host(login server):{}, fd:{}", port, target, connfd);
    if (connfd < 0) throw CheckResult::NetConnectFail;

    cli->onConnection = [](const hv::SocketChannelPtr& channel) {
        std::string peeraddr = channel->peeraddr();
        if (channel->isConnected()) {
            std::string magic = NetIO::setting_["MagicCheck"];
            channel->write(magic);
            clog("connected to:{}, connfd:{}, send magic:", peeraddr.c_str(), channel->fd(), magic);
        } else {
            clog("disconnected to:{}, connfd:{}", peeraddr.c_str(), channel->fd());
        }
    };

    hv::TimerID timer = cli->loop()->setTimeout(2500, [this](hv::TimerID timerid){
        clog("throw error: {}", as_str(CheckResult::LoginServNoReply));
        this->get_state()->set_current(state::net::Offline::instance());
        //std::terminate();
    });

    cli->onMessage = [timer, client = this->conn_](const hv::SocketChannelPtr& channel, hv::Buffer* buf) {
        client->loop()->killTimer(timer);
        clog("check reply, len:{}, content:{}, kill timer:{}", (int)buf->size(), (char*)buf->data(), timer);
    };

    // reconnect: 10, 20, 30, 40... 90, 100, 100...
    hv::ReconnectInfo reconn;
    reconn.min_delay = 10000;
    reconn.max_delay = 100000;
    reconn.delay_policy = 10;
    cli->setReconnect(&reconn);
    cli->start();
}

void NetIO::start() noexcept
{
    // no heart beat
    //int beat_init = NetIO::setting_["BeatCountInit"];
    //std::atomic_int* beat = new std::atomic_int(beat_init);
    //this->conn_->loop()->setInterval(1000, 
    //    [&beat, &channal = this->conn_->channel](hv::TimerID timerid) {
    //        beat->fetch_sub(1);
    //        if (*beat == 0) {
    // 
    //            channal->send(NetIO::setting_["BaseBeat"].dump());
    //        }

    //});

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
    // check setting 


}

void UserIO::start() noexcept
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
    this->rendee_ = new sol::state();
    this->log_ = this->setting_["StartWithLog"];

    // bind funtion with lua state and show login cartoon 
    this->init_state(this->rendee_);   
    
    return this;
}


void Render::init_state(sol::state* lua) noexcept
{
    // open all libraris
    lua->open_libraries(
        sol::lib::base, sol::lib::coroutine, sol::lib::math,
        sol::lib::package, sol::lib::string, sol::lib::table);
    
    // create global table
    lua->create_named_table("TaskQue");
    lua->create_named_table("LogCache");
    lua->operator[]("ThreadId") = lua->create_table_with(
        "U", Recvable::U,
        "N", Recvable::N,
        "C", Recvable::C);

    // regist function and load resource
    this->regist_act_to(lua);
    this->load_resource_to(lua);
}

void Render::check()
{
    this->get_rendee()->safe_script_file(R"(D:\GitBranch\GitHub\Trajectory\resource\paint\check.lua)");
}

#undef max
void Render::start() noexcept
{
    this->submit_task("OpenAnimation", 600, 600, sol::nil, sol::nil, 600, sol::nil);
    
    this->eloop_ = new std::thread([this]() {
        const json& screen = this->setting_["MainScreen"];
        int width = screen["Width"], hight = screen["Hight"];
        int fps = screen["Fps"];
        int expect = 1000 / fps;
        initgraph(width, hight);
        while (true)
        {
            auto srt = std::chrono::steady_clock::now();
     
            // start render, draw a frame
            if (bool ok = this->draw(); not ok) break;
        
            if (auto diff = expect - std::chrono::duration_cast
                            <std::chrono::milliseconds>(std::chrono::steady_clock::now() - srt).count();
                diff > 0) {
                Sleep(diff);
            } else
                continue;
        }
    });
    this->eloop_->detach();
    clog("Render thread start success");    
}

#undef max  // to deal with a complie error
void Render::stop() noexcept
{
    // set on crush flag for draw()
    this->frame_.store(std::numeric_limits<uint64_t>::max());
    
}

void Render::on_msg(const Recvable sender, Message&& m)
{

}

void Render::send_msg_to(const Recvable target, const Recvable self, Message&& m)
{
    
}

bool Render::draw() const noexcept
{
    if (this->frame_ != std::numeric_limits<uint64_t>::max()) {
        cleardevice();
        BeginBatchDraw();
        Render::s_mutex.lock();
        this->rendee_->operator[]("Render")["StepRender"].call();
        if (this->log_) this->rendee_->operator[]("Render")["ShowLog"].call();
        Render::s_mutex.unlock();
        EndBatchDraw();
        return true;
    }
    else {
        closegraph();
        return false;
    }
}

template<typename ...Args>
void Render::submit_task(const std::string& index, const Args& ...args) noexcept
{
    Render::s_mutex.lock();
    if constexpr (sizeof...(Args) == 0)
        this->rendee_->operator[]("Render")["AddRenderTask"].call(index);
    else {
        this->rendee_->operator[]("Render")["AddRenderTask"].call(index, args ...);
    }
    Render::s_mutex.unlock();
}

void Render::submit_log(Recvable self, const std::string& info) noexcept
{
    Render::s_mutex.lock();
    this->rendee_->operator[]("LogCache")[static_cast<size_t>(self)] = info;
    Render::s_mutex.unlock();
}

void Render::clear_task() noexcept
{
    Render::s_mutex.lock();
    this->rendee_->operator[]("Render").operator[]("ClearTask").call();
    Render::s_mutex.unlock();
}

void Render::regist_act_to(sol::state* lua) noexcept
{
    lua->create_named_table("Set",
        "SetLineStyle", [](int style, int thickness = 1) { setlinestyle(style, thickness); },
        "SetLineColor", setlinecolor,
        "SetFillStyle", [](int style, long hatch = 0L) { setfillstyle(style, hatch); },
        "SetFillColor", setfillcolor,
        "SetTextStyle", [](int height, int width, std::string&& front) {settextstyle(height, width, std::wstring(front.begin(), front.end()).c_str()); }
    );

    lua->create_named_table("Act",
        "Xyout",    [](int x, int y, std::string&& text) { outtextxy(x, y, std::wstring(text.begin(), text.end()).c_str()); },
        "Circle",   circle,
        "Line",     line
    );

    lua->create_named_table("LineStyle",
        "Solid",    PS_SOLID,
        "Dash",     PS_DASH,
        "Dot",      PS_DOT,
        "DashDot",  PS_DASHDOT
    );

    lua->create_named_table("FillStyle",
        "Solid",    BS_SOLID,
        "Null",     BS_NULL,
        "Hatched",  BS_HATCHED
    );

    lua->create_named_table("Hatch",
        "Cube",     HS_CROSS,
        "DiagCube", HS_DIAGCROSS
    );
}

void Render::load_resource_to(sol::state* lua)
{
    std::string path = Render::setting_["ResourcePath"];
    path = path.append("\\paint\\");

    auto load_exact_file = [&lua, &path](std::string_view filename, const std::string& modname) {
        if (not lua->require_file(modname, path.append(filename)).valid())
            clog("fail to require resource file: {}", path),
            throw CheckResult::LoadResourceFail; 
        else 
            path.erase(path.find(filename), path.length());
    };
    load_exact_file("color.lua",    "Color"),
    load_exact_file("ani.lua",      "Ani"),
    load_exact_file("object.lua",   "Object"),
    load_exact_file("rscript.lua",  "Render");

    clog("load all resource file success");
}