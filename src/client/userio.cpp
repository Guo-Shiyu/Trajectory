#include "../../header/client/userio.h"

SINGLETON_IMPL(UserIO)

UserIO* UserIO::lazy_init() noexcept
{
	this->eloop_ = new hv::EventLoopThread();
	this->state_ = new SelfState(this);
	this->state_->into(state::uio::SignIn::instance());
	return this;
}

UserIO* UserIO::ensure() noexcept
{
	assert(this->configer()["Ensure"]["UioEnsure"].call().valid());
	return this;
}

UserIO* UserIO::start() noexcept
{
	this->eloop_->loop()->setInterval(10,	// check user input every 10ms
		[&](hv::TimerID id) noexcept
		{
			if (ExMessage exmsg; peekmessage(&exmsg, (BYTE)255U, true))
				if (exmsg.message == WM_CHAR)
				{
					this->notify(ThreadId::R, "InputLog", ArgsPackBuilder::create(exmsg));
					this->mapper_(toascii(exmsg.vkcode));
				}
		});
	this->eloop_->start();
	return this;
}

UserIO* UserIO::panic() noexcept
{
	this->eloop_->stop();
	return this;
}

KeyMap UserIO::SignInMap = {
	{'h', []() {	Dispatcher::instance()->dispatch(ThreadId::N, ThreadId::U, "RequestRooms", std::nullopt);	}}
};

