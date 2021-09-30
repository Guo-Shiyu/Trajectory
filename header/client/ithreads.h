#pragma once

#include "../hv/json.hpp"
#include "../hv/singleton.h"
#include "../log.h"

#include "icommunication.h"
#include "iluavm.h"

using json = nlohmann::json;

template<typename S, typename T>
class WorkThread : virtual public iInit, virtual public iMsg, virtual public iConfig
{
    public:
    using SelfState = S;
    using SelfThread = T;
    
    protected:
    S*  state_;
    T*  eloop_;

    public:
    WorkThread() : state_(nullptr), eloop_(nullptr) {}
    
    WorkThread(const WorkThread& ) = delete;
    WorkThread& operator = (const WorkThread& ) = delete;
    
    S* state() noexcept { return this->state_; }
    T* loop() noexcept { return this->eloop_; }

    virtual WorkThread* start() noexcept   = 0;
    virtual WorkThread* panic() noexcept   = 0;
    //virtual WorkThread* stop()  noexcept   = 0;

    virtual ~WorkThread() {};
};



