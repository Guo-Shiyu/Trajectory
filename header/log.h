#pragma once

#include "hv/htime.h"

#include <iostream>
#include <string>
#include <fstream>
#include <format>
#include <queue>
#include <source_location>

#define LOG_SUGGESTED_MAX_LEN 128U
#define LOG_QUEUE_SIZE 64U

namespace Logger
{

    extern std::queue<std::string> cache_;

    // thread unsafe log
    template <typename Fmt, typename... Args>
    inline void log_gen(Fmt f, const std::source_location &&loc, Args &&...arg)
    {
        std::string logstr{}; logstr.reserve(LOG_SUGGESTED_MAX_LEN);
        auto cur = datetime_now();
        logstr.append(
            std::format("\n - :: [{:2}:{:2}:{:2}:{:4}]  file: {}\n - :: line:{:<4} function: {:<16}\n - >> ",
                        cur.hour, cur.min, cur.sec, cur.ms,
                        loc.file_name(), loc.line(), loc.function_name()));

        cache_.push(std::move(logstr.append(std::format(f, arg...))));

#ifdef _DEBUG
        std::cout << cache_.back() << std::endl;
#endif // _DEBUG

        while (cache_.size() >= LOG_QUEUE_SIZE)
            cache_.pop();
    }

    // print log to console or store log to file
    inline void log_dump(std::fstream *f = nullptr)
    {
        std::cout << "========================= Log Dump ===========================" << std::endl;
        if (f == nullptr || not f->good())
        {
            while (cache_.size() > 0)
                std::cout << cache_.front() << '\n', cache_.pop();
            std::cout << std::endl;
        }
        else
        {
            while (cache_.size() > 0)
                *f << cache_.front(), cache_.pop();
        }
    }
}

#define clog(fmt, ...) \
    Logger::log_gen(fmt, std::source_location::current(), __VA_ARGS__)