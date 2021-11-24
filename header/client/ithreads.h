#pragma once

#include "../hv/json.hpp"
#include "../hv/singleton.h"
#include "../log.h"

#include "ilpc.h"
#include "iluavm.h"

using json = nlohmann::json;

template<typename S, typename T>
class WorkThread : virtual public iInit, virtual public iConfig
{
public:
    using SelfState = S;
    using SelfThread = T;
    
public:
    S*  State;

protected:
    T*  eloop_;

public:
    WorkThread() : State(nullptr), eloop_(nullptr) {}
    
    WorkThread(const WorkThread& ) = delete;
    WorkThread& operator = (const WorkThread& ) = delete;
    
    T* loop() noexcept { return this->eloop_; }

    virtual WorkThread* start() noexcept   = 0;
    virtual WorkThread* panic() noexcept   = 0;

    // a work thread never stop unless crushed  
    // virtual WorkThread* stop()  noexcept   = 0;

    virtual ~WorkThread() {};
};



