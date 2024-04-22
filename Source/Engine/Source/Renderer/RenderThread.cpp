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

namespace Koala
{
    Logger logger("RenderThread");
    bool RenderThread::Initialize()
    {
        auto renderer = IModule::Get<Config>().GetSettingStrWithAutoSaving("render.renderer", "vulkan", true);
        auto avaliable_renderers = RHI::GetAvaliableRHIs();

        if (avaliable_renderers.count(renderer) == 0)
        {
            logger.error("Requested renderer unavaliable: {}", renderer);

            return false;
        }

        render = RHI::GetRHI(renderer);
        render->PreInit_MainThread();
        return true;
    }

    bool RenderThread::Shutdown()
    {
        render->PostShutdown_MainThread();
        return true;
    }

    void RenderThread::Tick(float delta_time)
    {
        if (!render->Tick_MainThread())
            Engine::Get().RequestEngineStop();
    }

    void RenderThread::Run()
    {
        logger.info("RenderThread is running.");

        logger.info("RenderThread: Initializing RHI");

        if (!render->Initialize_RenderThread())
        {
            logger.error("RHI: Failed to initialize!");

            logger.info("RenderThread: Shutdowning RHI");
            render->Shutdown_RenderThread();
            logger.info("RenderThread is stopping.");

            Engine::Get().RequestEngineStop();

            thread_has_initerr = true;

            {
                std::lock_guard lock(mutex_renderthread_stop);
                cv_renderthread_stop.notify_all();
            }

            {
                std::unique_lock lock(mutex_render_ready_or_initerr);
                cv_render_ready_or_initerr.notify_all();
            }
            return;
        }

        logger.info("RenderThread: RHI Initialized");

        {
            std::lock_guard lock_guard(mutex_render_ready_or_initerr);
            cv_render_ready_or_initerr.notify_all();
        }

        while (!Engine::Get().IsEngineExitRequested())
        {
            // TODO: Render!!!!!!!!!!!!!
        }

        logger.info("RenderThread: Shutdowning RHI");
        render->Shutdown_RenderThread();
        logger.info("RenderThread is stopping.");

        {
            std::lock_guard lock(mutex_renderthread_stop);
            cv_renderthread_stop.notify_all();
        }
    }
}
