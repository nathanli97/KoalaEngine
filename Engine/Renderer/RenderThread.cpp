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


#include "../Include/RenderThread.h"

#include <spdlog/spdlog.h>

#include "Config.h"
#include "Engine.h"
#include "../RenderHI/VulkanRHI/VulkanRHI.h"

namespace Koala
{
    bool RenderThread::Initialize()
    {
        auto renderer = IModule::Get<Config>().GetSettingStrWithAutoSaving("render.renderer", "vulkan", true);
        auto avaliable_renderers = RenderHI::GetAvaliableRenderHIs();

        if (avaliable_renderers.count(renderer) == 0)
        {
            spdlog::error("Requested renderer unavaliable: {}", renderer);

            return false;
        }

        render = RenderHI::GetRHI(renderer);
        return true;
    }

    bool RenderThread::Shutdown()
    {
        return true;
    }

    void RenderThread::Tick(float delta_time)
    {
    }

    void RenderThread::Run()
    {
        spdlog::info("RenderThread is running.");

        spdlog::info("RenderThread: Initializing RHI");

        if (!render->Initialize())
        {
            spdlog::error("RHI: Failed to initialize!");

            spdlog::info("RenderThread: Shutdowning RHI");
            render->Shutdown();
            spdlog::info("RenderThread is stopping.");

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

        spdlog::info("RenderThread: RHI Initialized");

        {
            std::lock_guard lock_guard(mutex_render_ready_or_initerr);
            cv_render_ready_or_initerr.notify_all();
        }

        while (render->Tick())
        {
            // TODO: Render!!!!!!!!!!!!!
        }

        spdlog::info("RenderThread: Shutdowning RHI");
        render->Shutdown();
        spdlog::info("RenderThread is stopping.");

        {
            std::lock_guard lock(mutex_renderthread_stop);
            cv_renderthread_stop.notify_all();
        }
    }
}
