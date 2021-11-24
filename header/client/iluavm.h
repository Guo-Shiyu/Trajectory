#pragma once

#include <fstream>
#include <sstream>
#include <optional>
#include <mutex>
#include <filesystem>
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
        std::call_once(flag, [vm = &iConfig::configer_]
                       { vm->open_libraries(sol::lib::base, sol::lib::coroutine,
                                            sol::lib::string, sol::lib::table); });
        return iConfig::configer_;
    }

    virtual ~iConfig(){};
};

static std::optional<std::string> read_to_string(std::string filename) noexcept
{
    std::ifstream in{filename, std::ios::in};
    if (in.is_open())
        return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    else
        return std::nullopt;
}

static void load_all_mod(sol::state *lua, const std::filesystem::path& directory) noexcept
{
    for (auto &it : std::filesystem::directory_iterator{directory})
    {   
        auto path = it.path();
        auto modname = path.filename().string();
		modname.erase(modname.begin() + modname.find('.'), modname.end());
        modname[0] = std::toupper(modname[0]);
        lua->require_file(modname, path.string());
        std::cout << "\t- Has Load Resource:" << modname << "\tIn Path: " << path.string() << std::endl;
    }
}