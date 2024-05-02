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
#include "Definations.h"

namespace Koala {
    enum class EEngineStage
    {
        UnInitialized,
        EarlyInitStage,
        InitStage,
        Running
    };
    class KoalaEngine
    {
    public:
        static bool Launch(int argc, char** argv)
        {
            if (!KoalaEngine::Get().Initialize(argc, argv))
            {
                KoalaEngine::Get().Shutdown();
                return false;
            }

            while(!KoalaEngine::Get().IsEngineExitRequested())
                KoalaEngine::Get().Tick();

            KoalaEngine::Get().Shutdown();
            return true;
        }
        NODISCARD EEngineStage GetEngineRunningStage() const
        {
            return engineStage;
        }
        static KoalaEngine& Get()
        {
            static KoalaEngine engine;
            return engine;
        }
        [[nodiscard]] bool IsEngineExitRequested() const
        {
            return requested_engine_stop;
        }

        void RequestEngineStop()
        {
            requested_engine_stop = true;
        }
    private:
        bool Initialize(int argc, char** argv);
        // Early init stage. All sub-systems, all threads should be created here.
        bool EarlyInitializeStage(int argc, char** argv);
        void CreateSubThreads();
        bool InitializeStage();
        void Tick();
        void Shutdown();

        EEngineStage engineStage {EEngineStage::UnInitialized};
        // TODO: This flag variable may be need to protect by LOCK
        bool requested_engine_stop = false;
    };
}
