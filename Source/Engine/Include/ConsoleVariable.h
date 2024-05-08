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

#include "ConsoleVariableBase.h"
#include "CVarManager.h"
#include "Memory/Allocator.h"

namespace Koala {
    template <typename InType>
    class TConsoleVariable final: public IConsoleVariableArithmetic
    {
    public:
        static_assert(std::is_arithmetic_v<InType>, "This type of TConsoleVariable can only accept arithmetic types.");
        typedef InType Type;
        TConsoleVariable(const std::string &inVarName, const Type& inDefaultValue, const std::string &inHelp, EConsoleVariableFlags inFlags = EConsoleVariableFlag::CVF_Default)
            : IConsoleVariableArithmetic(inFlags, inVarName, inHelp)
        {
            Set(inDefaultValue);
            RegisterConsoleVariable();
        }

        Type Get()
        {
            return IConsoleVariable::Get<Type>();
        }

        void Set(Type inValue)
        {
            return IConsoleVariable::Set(inValue);
        }
        
        TConsoleVariable() = delete;
        TConsoleVariable(const TConsoleVariable&) = delete;
        TConsoleVariable(TConsoleVariable&&) = delete;
        const TConsoleVariable& operator=(const TConsoleVariable&) = delete;
        TConsoleVariable&& operator=(TConsoleVariable&&) = delete;
    private:
        FORCEINLINE void RegisterConsoleVariable()
        {
            CVarManager::Get().RegisterConsoleVariable(this);
        }
    };
    
    template <>
    class TConsoleVariable<std::string> final: public IConsoleVariableString
    {
    public:
        typedef std::string Type;
        TConsoleVariable(const std::string &inVarName, const Type& inDefaultValue, const std::string &inHelp, EConsoleVariableFlags inFlags = EConsoleVariableFlag::CVF_Default)
            : IConsoleVariableString(inFlags, inVarName, inHelp)
        {
            RegisterConsoleVariable();
        }

        std::string Get()
        {
            return IConsoleVariable::Get<std::string>();
        }

        void Set(const std::string & inValue)
        {
            return IConsoleVariable::Set(inValue);
        }
        TConsoleVariable() = delete;
        TConsoleVariable(const TConsoleVariable&) = delete;
        TConsoleVariable(TConsoleVariable&&) = delete;
        const TConsoleVariable& operator=(const TConsoleVariable&) = delete;
        TConsoleVariable&& operator=(TConsoleVariable&&) = delete;
    private:
        FORCEINLINE void RegisterConsoleVariable()
        {
            CVarManager::Get().RegisterConsoleVariable(this);
        }
    };
}
