#pragma once

#include <string>
#include <format>
#include <queue>
#include <source_location>

namespace Logger {

    extern std::queue<std::string> log_;

    template<typename Fmt, typename ... Args>
    void log_gen(Fmt&& f, const std::source_location&& loc, Args&& ...arg)
    {
        std::string str{};
        str.reserve(128U);
        auto cur = datetime_now();
        
            str.append(std::format("-- {:2}:{:2}:{:2}:{:4}\t-{:13} :{:3} :{:15}:, \n\t--info:", 
                       cur.hour, cur.min, cur.sec, cur.ms,
                       loc.file_name(), loc.line(), loc.function_name()));
        log_.push(str.append(std::format(f, arg...)));
        
    }

    // print log to console
    void log_dump();
}

#define clog(fmt, ...) \
Logger::log_gen(fmt, std::source_location::current(), __VA_ARGS__)