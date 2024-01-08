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


#pragma once
#include <assert.h>
#include <map>
#include <set>
#include <string>


namespace Koala
{
    class CmdParser
    {
    public:
        inline static void Initialize(int argc, char **argv)
        {
            assert(!cmd_parser);
            cmd_parser = new CmdParser(argc, argv);
        }
        inline static CmdParser& Get()
        {
            return *cmd_parser;
        }

        [[nodiscard]] bool HasArg(const std::string& arg) const
        {
            return args.count(arg) != 0;
        }

        [[nodiscard]] std::string GetArgStr(const std::string& arg) const
        {
            if (!HasArg(arg))
                return "";
            return args[arg];
        }
    private:
        std::map<std::string, std::string> args;
        explicit CmdParser(int argc, char **argv);
        static CmdParser* cmd_parser;
    };
}
