#include "../../header/client/userio.h"

SINGLETON_IMPL(UserIO)

UserIO* UserIO::lazy_init() noexcept
{
	this->state_ = new SelfState(this);
	this->eloop_ = new hv::EventLoopThread();
	this->state_->into(state::uio::Both::instance());
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
		[&](hv::TimerID id)
		{
			if (ExMessage exmsg; peekmessage(&exmsg, (BYTE)255U, true))
				if (this->kfiliter()(exmsg))
					this->kreactor()(exmsg), this->kexpect()(std::move(exmsg));
		});
	this->eloop_->start();
	return this;
}

UserIO* UserIO::panic() noexcept
{
	this->eloop_->stop();
	return this;
}
