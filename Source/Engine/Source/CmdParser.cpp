// Copyright 2023 Li Xingru
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the “Software”), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "CmdParser.h"

#include <spdlog/spdlog.h>

#include <Core.h>

namespace Koala
{
    CmdParser *CmdParser::cmd_parser{nullptr};
    CmdParser::CmdParser(int argc, char** argv)
    {
        std::string curr_arg;
        std::string cmdline;
        for (int i = 1; i < argc; i++)
        {
            std::string arg = argv[i];
            cmdline += argv[i];
            cmdline += " ";
            const bool arg_with_short = StringTool::startswith(arg, "-");
            const bool arg_with_long = StringTool::startswith(arg, "--");
            if (
                arg_with_short ||
                arg_with_long
                )
            {
                const int substr_start = arg_with_long ? 2 : 1;
                arg = arg.substr(substr_start);

                auto position_of_equal = arg.find('=');
                if (position_of_equal != std::string::npos)
                {
                    auto param = arg.substr(position_of_equal);
                    auto key = arg.substr(0, position_of_equal);

                    if (args.count(key) != 0)
                    {
                        spdlog::warn("Found redundent arg {}, ignored", arg);
                    }
                    else
                    {
                        args.emplace(key, param);
                    }
                } else
                {
                    if (args.count(arg) != 0)
                    {
                        spdlog::warn("Found redundent arg {}, ignored", arg);
                    } else
                    {
                        curr_arg = arg;
                        args.emplace(arg, "");
                    }
                }
            } else if (!curr_arg.empty())
            {
                args[curr_arg] = arg;
                curr_arg.clear();
            } else
            {
                spdlog::warn("Unexpected arg {}, ignored", arg);
            }
        }
        if (cmdline.empty())
            spdlog::info("Engine running with no commandline");
        else
            spdlog::info("Engine running with commandline {}", cmdline);

    }
}