#pragma once

#include <unordered_map>

#include "../client/interface.hpp"
#include "../hv/htime.h"

class Message 
{
    public:
    size_t send_;
    size_t recv_;

    size_t msgt_;

};

class MsgDispatcher 
{
    private:
    std::unordered_map<Recvable, MsgInterface*> map_;
};

enum class CheckResult : size_t
{
    NetConnectFail = 0,
    LoginServNoReply = 1,
};

template <typename Key, typename Value, std::size_t Size>
struct Map {
    std::array<std::pair<Key, Value>, Size> data;

    [[nodiscard]] // should not discard return value
    constexpr Value at(const Key &key) const noexcept {
        const auto itr =
            std::find_if(std::begin(data), std::end(data),
                         [&key](const auto &v) { return v.first == key; });
        if (itr != std::end(data)) {
            return itr->second;
        } else {
            return "unkown error condition";
        }
    }
};

using namespace std::literals::string_view_literals;
static constexpr std::array<std::pair<CheckResult, std::string_view>, 8U> kerr_map{
    {   {CheckResult::NetConnectFail,       "Open \"clicfg.json\" failed "},
        {CheckResult::LoginServNoReply,     "Login Server no reply"},
        {CheckResult::NetConnectFail, "Open \"clicfg.json\" failed "},
        {CheckResult::NetConnectFail, "Open \"clicfg.json\" failed "},
        {CheckResult::NetConnectFail, "Open \"clicfg.json\" failed "},
        {CheckResult::NetConnectFail, "Open \"clicfg.json\" failed "},
        {CheckResult::NetConnectFail, "Open \"clicfg.json\" failed "},
        {CheckResult::NetConnectFail, "Open \"clicfg.json\" failed "}   }};

// i need [!(no_mangeling)]
std::string_view as_str(const CheckResult sv);
