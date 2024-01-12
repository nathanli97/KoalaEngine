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
#include "../RenderHI/VulkanRHI/VulkanRHI.h"

namespace Koala
{
    bool RenderThread::Initialize()
    {
        auto renderer = IModule::Get<Config>().GetSettingStr("render.renderer", "vulkan");
        auto avaliable_renderers = RenderHI::GetAvaliableRenderHIs();

        if (avaliable_renderers.count(renderer.value()) == 0)
        {
            spdlog::error("Requested renderer unavaliable: {}", renderer.value());

            return false;
        }

        render = RenderHI::GetRHI(renderer.value());
        spdlog::info("The MainThread Part of RenderThread initialization is completed. Waiting for RT running.");
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
        render->Initialize();
        spdlog::info("RenderThread: RHI Initialized");

        {
            std::lock_guard lock_guard(mutex_render_ready);
            cv_render_ready.notify_all();
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
