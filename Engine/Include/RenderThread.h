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

#include "ThreadedModule.h"

namespace Koala::RenderHI
{
    struct RenderHI;
}

namespace Koala {
    class RenderThread: public IThreadedModule {
    public:
        bool Initialize() override;
        bool Shutdown() override;
        void Tick(float delta_time) override;
        void Run() override;

        // Wait for rendering system is ready.
        // This function will block executes until rendering system is initialized completedly and ready to render.
        // This function can be called in any-thread except RenderThread.
        void WaitForRenderReady()
        {
            std::unique_lock lock(mutex_render_ready);
            cv_render_ready.wait(lock);
        }

        void WaitForRTStop()
        {
            std::unique_lock lock(mutex_renderthread_stop);
            cv_renderthread_stop.wait(lock);
        }

        RenderThread(): IThreadedModule() {}
    private:
        RenderHI::RenderHI *render = nullptr;

        std::mutex mutex_render_ready;
        std::condition_variable cv_render_ready;

        std::mutex mutex_renderthread_stop;
        std::condition_variable cv_renderthread_stop;
    };
}

