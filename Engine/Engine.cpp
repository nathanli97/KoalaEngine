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

#include "Engine.h"


#include "Config.h"
#include "Core.h"
#include "EngineVersion.h"
#include "RenderThread.h"
#include "RenderHI/RenderHI.h"


namespace Koala
{
    Logger logger_engine("Engine");
    bool Engine::Initialize(int argc, char** argv)
    {
        using namespace Koala;

        CmdParser::Initialize(argc, argv);

        if (CmdParser::Get().HasArg("debug"))
        {
            spdlog::set_level(spdlog::level::debug);
            logger_engine.warning("LogLevel set to DEBUG");
        }

        Scripting::Initialize();

        void *init_script = Scripting::LoadScriptFromDisk("init");

        if (init_script)
        {
            logger_engine.debug("Executing pre_init script");
            Scripting::ExecuteFunctionNoArg(init_script, "pre_init");
        }
        else
        {
            logger_engine.error("Init script load failed.");
            return false;
        }

        // Initialize core modules
        IModule::Get<Config>().Initialize();
        if (CmdParser::Get().HasArg("printcfg"))
        {
            IModule::Get<Config>().PrintAllConfigurations();
        }
        // Initialize render-thread

        IModule::Get<RenderThread>().Initialize();
        IModule::Get<RenderThread>().CreateThread();

        ISingleton::Get<ModuleManager>().InitializeModules();


        if (!IModule::Get<RenderThread>().WaitForRenderReady())
        {
            logger_engine.error("Rendering System has error");
            return false;
        }

        logger_engine.debug("Executing post_init script");
        Scripting::ExecuteFunctionNoArg(init_script, "post_init");
        Scripting::UnloadScript(init_script);

        logger_engine.info("Engine initialized");
        logger_engine.info("Welcome to KoalaEngine {}.{}.{} ({})",
            KOALA_ENGINE_VER_MAJOR,
            KOALA_ENGINE_VER_MINOR,
            KOALA_ENGINE_VER_PATCH,
            KOALA_ENGINE_VER_CODENAME);
        return true;
    }
    void Engine::Tick()
    {
        // TODO: Pass actual deltatime to the function.
        IModule::Get<RenderThread>().Tick(0);
    }

    void Engine::Shutdown()
    {
        IModule::Get<RenderThread>().WaitForRTStop();
        IModule::Get<RenderThread>().Shutdown();
        ISingleton::Get<ModuleManager>().ShutdownModules();
        IModule::Get<Config>().Shutdown();
        Scripting::Shutdown();
        logger_engine.info("Engine is exiting");
    }

}
