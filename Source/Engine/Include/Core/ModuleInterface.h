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
#include <list>

#include "SingletonInterface.h"

namespace Koala {
    struct IModule: ISingleton {
    public:
        // virtual destructor
        virtual ~IModule() = default;

        // Module startup/shutdown functions.
        // Those functions will be called in MainThread.
        virtual bool Initialize_MainThread() = 0;
        virtual bool Shutdown_MainThread() = 0;

        // Module Tick function.
        // Tick function will be called once every frame.

        // delta_time: in ms.
        virtual void Tick(float delta_time) = 0;
    };
    class ModuleManager: public ISingleton
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(ModuleManager)
        // Register a new module
        // the modules will initializes in registered order.
        void RegisterModule(IModule* module);

        void InitializeModules();
        void TickModules(float delta);
        void ShutdownModules();
    private:
        std::list<IModule*> modules;
    };
}
