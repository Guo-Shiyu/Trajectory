#include "../../header/client/icommunication.h"
#include "../../header/client/netio.h"
#include "../../header/client/userio.h"
#include "../../header/client/render.h"
#include "../../header/client/client.h"

class Dispatcher;
void iMsg::response(const ThreadId sender, ProcIndex i, std::optional<ArgsPack> args) noexcept
{
    this->fnmap_->at(i)(args);
}

void iMsg::notify(const ThreadId target, ProcIndex i, std::optional<ArgsPack> args) noexcept
{
    Dispatcher::instance()->dispatch(target, ThreadId::C, i, args);
}

Dispatcher *Dispatcher::lazy_init() noexcept
{
    this->map_.try_emplace(ThreadId::C, Client::i_say_there_would_be_light());
    this->map_.try_emplace(ThreadId::N, NetIO::instance());
    this->map_.try_emplace(ThreadId::R, Render::instance());
    this->map_.try_emplace(ThreadId::U, UserIO::instance());
    return this;
}

void Dispatcher::destory()
{
    // unimplenmented!
}

void Dispatcher::dispatch(const ThreadId target, const ThreadId sender, ProcIndex index, std::optional<ArgsPack> args) const noexcept
{
    this->map_.at(target)->response(sender, index, args);
}

Dispatcher *Dispatcher::instance() noexcept
{
    static Dispatcher dis{};
    static std::once_flag flag{};
    std::call_once(flag, [&]()
                   { dis.lazy_init(); });
    return &dis;
}