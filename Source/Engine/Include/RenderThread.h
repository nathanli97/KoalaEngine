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
#include <condition_variable>

#include "Core/ThreadedModule.h"

namespace Koala::RHI
{
    struct IRenderHardware;
}

namespace Koala {
    class RenderThread: public IThreadedModule {
    public:
        KOALA_IMPLEMENT_SINGLETON(RenderThread)
        // Caller: MT, BEFORE RT is created
        bool Initialize_MainThread() override;
        // Caller: MT, AFTER RT is stopped
        bool Shutdown_MainThread() override;

        // Caller: MT
        void Tick_MainThread(float delta_time) override;

        // Caller: RT
        void Run() override;

        // Wait for rendering system is ready, or render thread is exited too early.
        // This function will block executes until rendering system is initialized completedly and ready to render.
        // This function can be called in any-thread except RenderThread.

        // Return FALSE if RenderThread is exited too early (Has ERROR).
        bool WaitForRenderReady()
        {
            std::unique_lock lock(mutexRenderReadyOrInitErr);
            cvRenderReadyOrInitErr.wait(lock);

            return !bRenderInitFailed;
        }

        void WaitForRTStop()
        {
            if (bRenderInitFailed)
                return;

            std::unique_lock lock(mutexRenderThreadStop);
            cvRenderThreadStop.wait(lock);
        }

        RenderThread(): IThreadedModule() {}
    private:
        RHI::IRenderHardware *rhi = nullptr;

        std::mutex mutexRenderReadyOrInitErr;
        std::condition_variable cvRenderReadyOrInitErr;

        std::mutex mutexRenderThreadStop;
        std::condition_variable cvRenderThreadStop;

        bool bRenderInitFailed = false;
    };
}

