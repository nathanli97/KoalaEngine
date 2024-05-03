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
#include <map>
#include <string>

#include "Core/Check.h"
#include "Core/SingletonInterface.h"
#include "ConsoleVariableBase.h"
#include "Core/ModuleInterface.h"


namespace Koala
{
    template <typename InType>
    class TConsoleVariable;

    class IConsoleVariable;
    
    class CVarManager final : protected IModule
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(CVarManager)
        template<typename T>
        friend class TConsoleVariable;

        bool Initialize_MainThread() override { return true; }
        bool Shutdown_MainThread() override { return true; }
        void Tick(float delta_time) override {}
    protected:
        void RegisterConsoleVariable(IConsoleVariable* inCVar)
        {
            std::lock_guard lock(lockForMapNameToCVar);
            check(!mapNameToCVar.contains(inCVar->name));
            mapNameToCVar.emplace(inCVar->name, inCVar);
        }
    private:
        std::map<std::string, IConsoleVariable*> mapNameToCVar;
        std::mutex lockForMapNameToCVar;
    };
}
