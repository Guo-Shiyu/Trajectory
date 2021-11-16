#pragma once

#include "iinit.h"
#include "assist.h"

#include <vector>
#include <array>
#include <initializer_list>
#include <string_view>
#include <functional>
#include <unordered_map>
#include <any>
#include <optional>

enum class ThreadId : size_t
{
    C = 0, // client
    R = 1, // render
    N = 2, // net io
    U = 3, // user io
};

template <typename Key, typename Value, std::size_t Size>
struct Map
{
    std::array<std::pair<Key, Value>, Size> data;

    // [[nodiscard]] // should not discard return value
    constexpr Value
    at(const Key &key) const noexcept
    {
        const auto itr =
            std::find_if(std::begin(data), std::end(data),
                         [&key](const auto &v)
                         { return v.first == key; });
        if (itr != std::end(data))
            return itr->second;
        else 
            // unreachable code 
            return "unkown error condition";
    }
};

using namespace std::literals::string_literals;
constexpr static std::array<std::pair<ThreadId, std::string_view>, 4U> 
    kthd_map{{
        {ThreadId::C, "Client"},
        {ThreadId::R, "Render"},
        {ThreadId::N, "NetIO"},
        {ThreadId::U, "UserIO"},
    }};

constexpr static std::string_view as_str(const ThreadId id)
{
    constexpr Map<ThreadId, std::string_view, kthd_map.size()>
        map = {{kthd_map}};
    return map.at(id);
}

template <typename Arg, typename C>
concept ArgsContainer = std::is_same<C, std::vector<Arg>>::value || std::is_same<C, std::list<Arg>>::value;

template <typename Arg, typename Container>
requires ArgsContainer<Arg, Container>
class ParamPackage
{
public:
    ParamPackage() : args_() {}

    ParamPackage(const std::initializer_list<Arg> &params)
    {
        args_.reserve(params.size());
        args_.insert(args_.begin(), params.begin(), params.end());
    }

    ~ParamPackage() = default;

    // set args
    ParamPackage *with(Arg &&arg) noexcept
    {
        this->args_.push_back(arg);
        return this;
    }

    // set args
    ParamPackage *with(const std::initializer_list<Arg> &args) noexcept
    {
        for (auto &arg : args)
            this->args_.push_back(arg);
        return this;
    }

    // get args container
    Container& args_pack() noexcept
    {
        return this->args_;
    }

    // create a new param package
    template <typename... Args>
    static std::shared_ptr<ParamPackage> create(Args&&... args) noexcept
    {
        return std::make_shared<ParamPackage>(std::initializer_list<Arg>{std::forward<Args&&>(args) ...});
    }

private:
    Container args_;
};

using ArgsPackBuilder = ParamPackage<std::any, std::vector<std::any>>;
using ArgsPack      = std::shared_ptr<ParamPackage<std::any, std::vector<std::any>>>;
using ProcIndex     = std::string_view;
using MsgHandler    = std::function<void(std::optional<ArgsPack>)>;
using CallMap       = std::unordered_map<ProcIndex, MsgHandler>;

namespace Dispatcher
{

// defined in 'dispather.cpp' 
extern std::unordered_map<ThreadId, CallMap> LpcMap;

// public interface : dispatch msg to other thread 
inline void dispatch(const ThreadId target, ProcIndex index, std::optional<ArgsPack> args) noexcept
{
    Dispatcher::LpcMap.at(target)			// get target thread's CallMap
                        .at(index)			// get target fn with designed index 
                      /*.call*/(args);		// call fn with args
}

}