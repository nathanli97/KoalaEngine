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
#include <memory>

namespace Koala::RHI
{
    enum class ECommandQueueType
    {
        PresentQueue,
        CommandQueue,
        GraphicsQueue,
        TransferQueue
    };
    struct RHICommandPool
    {
        const void* GetPlatformNativePointer() const {return this;}
        void* GetPlatformNativePointer() {return this;}
    };

    struct RHICommandBuffer
    {
        const void* GetPlatformNativePointer() const {return this;}
        void* GetPlatformNativePointer() {return this;}
    };

    struct RHICommandQueue
    {
        const void* GetPlatformNativePointer() const {return this;}
        void* GetPlatformNativePointer() {return this;}
    };
    
    typedef std::shared_ptr<RHICommandPool>   CommandPoolRef;
    typedef std::shared_ptr<RHICommandBuffer> CommandBufferRef;

    // The queue should be created when RHI initializes, and destroy when RHI shutdown.
    // No need to use shared_ptr.
    typedef RHICommandQueue*  CommandQueueRef;
}
