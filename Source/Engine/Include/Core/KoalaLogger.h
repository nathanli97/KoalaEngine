//Copyright 2024 Li Xingru
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
//associated documentation files (the “Software”), to deal in the Software without restriction,
//including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
//and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
//so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial
//portions of the Software.
//
//THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
//FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#pragma once
#include <string>
#include <utility>
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>

namespace Koala
{
    class Logger
    {
    public:
        inline explicit Logger (const std::string &log_cat)
        {
            auto static sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
            logger = new spdlog::logger(log_cat, sink);
        }
        ~Logger()
        {
            delete logger;
        }
        template <typename ...Args>
        inline void debug(spdlog::format_string_t<Args...> fmt, Args... args)
        {
            logger->debug(fmt, std::forward<Args>(args)...);
        }
        template <typename ...Args>
        void info(spdlog::format_string_t<Args...> fmt, Args... args)
        {
            logger->info(fmt, std::forward<Args>(args)...);
        }
        template <typename ...Args>
        void warning(spdlog::format_string_t<Args...> fmt, Args... args)
        {
            logger->warn(fmt, std::forward<Args>(args)...);
        }
        template <typename ...Args>
        void error(spdlog::format_string_t<Args...> fmt, Args... args)
        {
            logger->error(fmt, std::forward<Args>(args)...);
        }
    private:
        spdlog::logger *logger = nullptr;
    };
}
