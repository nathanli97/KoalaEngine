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


#include "RenderThread.h"

#include <spdlog/spdlog.h>

#include "Config.h"
#include <Core.h>
#include "../RHI/VulkanRHI/VulkanRHI.h"
#include "Core/ThreadManager.h"
#include "Renderer/Core/RenderCmdProcessor.h"

namespace Koala
{
    Logger logger("RenderThread");
    bool RenderThread::Initialize_MainThread()
    {
        auto renderer = Config::Get().GetSettingAndWriteDefault("render.renderer", "vulkan", true);
        auto availableRenderRHIs = RHI::GetAvaliableRHIs();

        if (!availableRenderRHIs.contains(renderer))
        {
            logger.error("Requested renderer unavaliable: {}", renderer);

            return false;
        }

        rhi = RHI::GetRHI(renderer);
        rhi->PreInit_MainThread();
        return true;
    }

    bool RenderThread::Shutdown_MainThread()
    {
        rhi->PostShutdown_MainThread();
        return true;
    }

    void RenderThread::Tick_MainThread(float delta_time)
    {
        if (!rhi->Tick_MainThread())
            KoalaEngine::Get().RequestEngineStop();
    }

    void RenderThread::Run()
    {
        // Render::Core::RenderCmdProcessor processor;
        // processor.AddTaskLambda([](){});
        ThreadTLS::Initialize(EThreadType::RenderThread);
        
        logger.info("Initializing RHI");

        if (!rhi->Initialize_RenderThread())
        {
            logger.error("RHI: Failed to initialize!");

            logger.info("RenderThread: Shutdowning RHI");
            rhi->Shutdown_RenderThread();
            logger.info("RenderThread is stopping.");

            KoalaEngine::Get().RequestEngineStop();

            bRenderInitFailed = true;

            {
                std::lock_guard lock(mutexRenderThreadStop);
                cvRenderThreadStop.notify_all();
            }

            {
                std::unique_lock lock(mutexRenderReadyOrInitErr);
                cvRenderReadyOrInitErr.notify_all();
            }
            return;
        }

        logger.info("RenderThread: RHI Initialized");

        {
            std::lock_guard lock_guard(mutexRenderReadyOrInitErr);
            cvRenderReadyOrInitErr.notify_all();
        }

        while (true)
        {
            if (KoalaEngine::Get().IsEngineExitRequested())
            {
                logger.info("Engine exit was requested. Trying to shutdown RenderThread...");
                break;
            }
            // TODO: Render!!!!!!!!!!!!!
        }

        logger.info("RenderThread: Shutdowning RHI");
        rhi->Shutdown_RenderThread();
        logger.info("RenderThread is stopping.");

        {
            std::lock_guard lock(mutexRenderThreadStop);
            cvRenderThreadStop.notify_all();
        }
    }
}
