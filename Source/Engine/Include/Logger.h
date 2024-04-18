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
#include <spdlog/spdlog.h>

namespace Koala
{
    class Logger
    {
    public:
        inline Logger (const std::string& log_cat): cat(log_cat) {}
        template <typename ...T>
        inline void debug(std::string in_fmt, T... args)
        {
            auto fmt = "[{}] " + in_fmt;
            spdlog::debug(fmt, cat, args...);
        };
        template <typename ...T>
        void info(std::string in_fmt, T... args)
        {
            auto fmt = "[{}] " + in_fmt;
            spdlog::info(fmt, cat, args...);
        };
        template <typename ...T>
        void warning(std::string in_fmt, T... args)
        {
            auto fmt = "[{}] " + in_fmt;
            spdlog::warn(fmt, cat, args...);
        };
        template <typename ...T>
        void error(std::string in_fmt, T... args)
        {
            auto fmt = "[{}] " + in_fmt;
            spdlog::error(fmt, cat, args...);
        };
    private:
        std::string cat;
    };
}
