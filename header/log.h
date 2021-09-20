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

namespace Logger {

    extern std::queue<std::string> logger;

    // thread unsafe log
    template<typename Fmt, typename ... Args> inline
    void log_gen(Fmt&& f, const std::source_location&& loc, Args&& ...arg)
    {
        std::string logstr{}; logstr.reserve(LOG_SUGGESTED_MAX_LEN);
        auto cur = datetime_now();
        logstr.append(std::format("\n - :: time: {:2}:{:2}:{:2}:{:4}  file: {}\n - :: line:{:<4} function: {:<16}\n - >> ", 
                   cur.hour, cur.min, cur.sec, cur.ms,
                   loc.file_name(), loc.line(), loc.function_name()));
        logger.push(std::move(logstr.append(std::format(f, arg...))));
        
        while(logger.size() >= LOG_QUEUE_SIZE)
            logger.pop();
    }

    // print log to console or store log to file
    inline void log_dump(std::fstream* f = nullptr)
    {
        if (f == nullptr || not f->good()) {
            while (logger.size() > 0)
                std::cout << logger.front() << '\n', logger.pop();
            std::cout << std::endl;
        }
        else {
            while (logger.size() > 0)
                *f << logger.front(), logger.pop();
        }
    }
}

#define clog(fmt, ...) \
Logger::log_gen(fmt, std::source_location::current(), __VA_ARGS__)