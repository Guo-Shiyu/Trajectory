#include "../../header/client/userio.h"

SINGLETON_IMPL(UserIO)

UserIO* UserIO::lazy_init() noexcept
{
	this->eloop_ = new hv::EventLoopThread();
	this->State = new SelfState(this);
	return this;
}

UserIO* UserIO::ensure() noexcept
{
	this->configer()["Ensure"]["UioEnsure"].call().valid();
	return this;
}

UserIO* UserIO::start() noexcept
{
	this->eloop_
		->loop()
		->setInterval(10,	// check user input every 10ms
			[&](hv::TimerID id) noexcept
			{
				if (ExMessage exmsg; peekmessage(&exmsg, (BYTE)255U, true))
					if (exmsg.message == WM_CHAR)
					{
						Dispatcher::dispatch(ThreadId::R, "InputLog", ArgsPackBuilder::create(exmsg));
						auto key = toascii(exmsg.vkcode);
						if (this->Mapper->contains(key))
							this->Mapper->at(key)();
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

