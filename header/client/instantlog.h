#pragma once

#include "interface.hpp"
#include "../log.h"

class InstantLog : public InitInterface
{
private:
	
	// log switch
	bool		onlog_;
	
	// cache for Uio, Nio, Clog
	LogCache* 	uio_;
	LogCache* 	net_;
	LogCache* 	clog_;

public:
	InstantLog() : onlog_(false), uio_(nullptr), net_(nullptr), clog_(nullptr) {}
	InstantLog* init_self() noexcept override final;
	void check();
	void start() noexcept;
	void stop() noexcept;
	bool running() const noexcept;
	void show() const noexcept;

	~InstantLog() {}
};
