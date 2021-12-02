#pragma once

#include <mutex>
#include <graphics.h>

#undef max
#include "../sol/config.hpp"
#include "../sol/forward.hpp"
#include "../sol/sol.hpp"

//#include "../hv/hstring.h"


class iLua
{
protected:
    sol::state vm_;
    iLua() : vm_() {}

public:
    sol::state *luavm() noexcept
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
    virtual iConfig *ensure() = 0;

    static sol::state &configer() noexcept
    {
        static std::once_flag flag{};
        std::call_once(flag, [vm = &iConfig::configer_]()
        { 
            vm->open_libraries(sol::lib::base, sol::lib::coroutine,
                                            sol::lib::string, sol::lib::table); 
        });
        return iConfig::configer_;
    }

    virtual ~iConfig(){};
};
