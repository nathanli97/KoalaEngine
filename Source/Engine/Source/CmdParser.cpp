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
        std::string currArg;
        std::string cmdline;
        for (int i = 1; i < argc; i++)
        {
            std::string arg = argv[i];
            cmdline += argv[i];
            cmdline += " ";
            const bool argWithShort = StringTool::startswith(arg, "-");
            const bool argWithLong = StringTool::startswith(arg, "--");
            if (
                argWithShort ||
                argWithLong
                )
            {
                const int substrStart = argWithLong ? 2 : 1;
                arg = arg.substr(substrStart);

                auto positionOfEqual = arg.find('=');
                if (positionOfEqual != std::string::npos)
                {
                    auto param = arg.substr(positionOfEqual);
                    auto key = arg.substr(0, positionOfEqual);

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
                        currArg = arg;
                        args.emplace(arg, "");
                    }
                }
            } else if (!currArg.empty())
            {
                args[currArg] = arg;
                currArg.clear();
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