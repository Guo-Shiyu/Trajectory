#pragma once

#undef max
#include "../sol/config.hpp"
#include "../sol/forward.hpp"
#include "../sol/sol.hpp"

class iLua 
{
    protected:
    sol::state vm_;
    iLua() : vm_() {}

    public:
    sol::state* luavm() noexcept
    {
        return &this->vm_;
    }

    virtual ~iLua() noexcept {}
};

class iConfig
{
    private:
    static sol::state configer_;

    protected:
    iConfig() = default;
    
    public:

    // ensure configuration correct
    virtual iConfig* ensure()     = 0;
    
    static sol::state& configer() noexcept
    {
        return iConfig::configer_;
    }
    
    virtual ~iConfig()          {};
};
