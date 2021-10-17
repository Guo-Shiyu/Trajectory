#pragma once

#include "../hv/TcpClient.h"
#include "states.h"

#include "ithreads.h"

class iNetIO : public WorkThread<StateMachine<iNetIO>, std::nullptr_t>
{
protected:
    hv::TcpClient* conn_;

public:
    iNetIO() : WorkThread(), conn_(nullptr) {}
    hv::TcpClient* connect() noexcept 
    {
        return this->conn_;
    }

    ~iNetIO() = default;
};

class NetIO : public iNetIO
{
    SINGLETON_DECL(NetIO)

    public:
    NetIO()     = default;
    ~NetIO()    = default;
    
    // init interface
    NetIO*  lazy_init() noexcept override final;

    // condig interface
    NetIO*  ensure() noexcept override final;

    // work thread interface
    NetIO*  start() noexcept override final;
    NetIO*  panic() noexcept override final;
};

class Protocal
{
private:
    static json& instance() noexcept;
public:
    static std::string take(std::string_view index) noexcept;
};