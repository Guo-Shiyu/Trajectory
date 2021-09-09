#pragma once

#include "../client/interface.h"

#include <queue>
#include <source_location>
#include <format>

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

class Logger
{
private:
    static std::queue<std::string> log_;
public:
    template<typename Fmt, typename ... Args>
    static void log_gen(Fmt&& f, std::source_location loc, Args&& ...arg)
    {
        static std::string str{ 128 };
        static std::mutex smtx;
        {
            smtx.lock();
            str.append(std::format("-{:13} :{:3} :{:15}:, \n\t--info:"), loc.file_name(), loc.line(), loc.function_name());
            log_.push(str.append(std::format(f, arg...)));
            str.clear();
            smtx.unlock();
        }
    }

    // print log to console
    static void log_dump();
};

#define clog(fmt, ...) \
Logger::log_gen(fmt, std::source_location::current(), __VA_ARGS__)