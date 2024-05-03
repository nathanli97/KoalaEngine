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

#include "KoalaEngine.h"


#include "Config.h"
#include "Core.h"
#include "EngineVersion.h"
#include "RenderThread.h"



namespace Koala
{
    Logger loggerEngine("KoalaEngine");
    bool KoalaEngine::Initialize(int argc, char** argv)
    {
        return EarlyInitializeStage(argc, argv) &&
            InitializeStage();
    }
    bool KoalaEngine::EarlyInitializeStage(int argc, char** argv)
    {
        using namespace Koala;
        engineStage = EEngineStage::EarlyInitStage;
        Logger loggerEngineEarlyInit("EngineEarlyInitialize");

        CmdParser::Initialize(argc, argv);

        if (CmdParser::Get().HasArg("debug"))
        {
            spdlog::set_level(spdlog::level::debug);
            loggerEngine.warning("LogLevel set to DEBUG");
        }

        // Initialize core modules
        Config::Get().Initialize_MainThread();
        if (CmdParser::Get().HasArg("PrintCFG"))
        {
            Config::Get().PrintAllConfigurations();
        }

        RenderThread::Get().Initialize_MainThread();
        ModuleManager::Get().InitializeModules();
        
        // Sub-threads should be created in end of engine early init stage.
        CreateSubThreads();
        return true;
    }

    void KoalaEngine::CreateSubThreads()
    {
        RenderThread::Get().CreateThread();
    }

    bool KoalaEngine::InitializeStage()
    {
        engineStage = EEngineStage::InitStage;
        Logger loggerEngineInit("EngineInitialize");

        Scripting::Initialize();

        void *init_script = Scripting::LoadScriptFromDisk("init");

        if (init_script)
        {
            loggerEngineInit.debug("Executing pre_init script");
            Scripting::ExecuteFunctionNoArg(init_script, "pre_init");
        }
        else
        {
            loggerEngineInit.error("Init script load failed. Tip: CurrentWorkingDirectory should be koala project's root directory. (KoalaEngine will attempt to load script from Source/KoalaEngine/Scripts/)");
        }
        
        if (!RenderThread::Get().WaitForRenderReady())
        {
            loggerEngineInit.error("Rendering System failed to initialize");
            return false;
        }

        loggerEngineInit.debug("Executing post_init script");
        Scripting::ExecuteFunctionNoArg(init_script, "post_init");
        Scripting::UnloadScript(init_script);

        loggerEngineInit.info("KoalaEngine initialized, Welcome to KoalaEngine {}.{}.{} ({})",
            KOALA_ENGINE_VER_MAJOR,
            KOALA_ENGINE_VER_MINOR,
            KOALA_ENGINE_VER_PATCH,
            KOALA_ENGINE_VER_CODENAME);

        engineStage = EEngineStage::Running;
        return true;
    }


    void KoalaEngine::Tick()
    {
        // TODO: Pass actual deltatime to the function.
        RenderThread::Get().Tick(0);
    }

    void KoalaEngine::Shutdown()
    {
        RenderThread::Get().WaitForRTStop();
        RenderThread::Get().Shutdown_MainThread();
        ModuleManager::Get().ShutdownModules();
        Config::Get().Shutdown_MainThread();
        Scripting::Shutdown();
        loggerEngine.info("KoalaEngine is exiting");
    }

}
