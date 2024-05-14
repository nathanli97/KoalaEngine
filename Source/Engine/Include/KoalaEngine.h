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
#include "Game/Scene.h"

namespace Koala {
    enum class EEngineStage
    {
        UnInitialized,
        EarlyInitStage,
        InitStage,
        Running,
        ExitRequested,
        Exiting
    };
    class KoalaEngine
    {
    public:
        static void Launch(int argc, char** argv)
        {
            auto &engine = KoalaEngine::Get();
            if (!engine.Initialize(argc, argv))
            {
                engine.Shutdown();
            }

            while(!engine.IsEngineExitRequested())
                engine.Tick();

            engine.Shutdown();
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
        NODISCARD FORCEINLINE bool IsEngineExitRequested() const
        {
            return engineStage.load(std::memory_order::acquire) >= EEngineStage::ExitRequested;
        }

        void RequestEngineStop()
        {
            if (engineStage.load(std::memory_order::acquire) < EEngineStage::ExitRequested)
                engineStage.store(EEngineStage::ExitRequested, std::memory_order::release);
        }
    private:
        bool Initialize(int argc, char** argv);
        // Early init stage. All sub-systems, all threads should be created here.
        bool EarlyInitializeStage(int argc, char** argv);
        void CreateSubThreads();
        bool InitializeStage();
        void Tick();
        void Shutdown();

        std::atomic<EEngineStage> engineStage {EEngineStage::UnInitialized};

        std::shared_ptr<Scene> currScene;
    };

    FORCEINLINE void LaunchKoala(int argc, char **argv)
    {
        KoalaEngine::Launch(argc, argv);
    }
}
