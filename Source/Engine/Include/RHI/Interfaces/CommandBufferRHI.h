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
#include "Core/SingletonInterface.h"
#include "RHI/CommandBufferResources.h"

namespace Koala::RHI
{
    
    class ICommandBufferInterface: public ISingleton
    {
    public:
        virtual ~ICommandBufferInterface() = default;
        /**
         * Get CommandQueue by given type.
         * The CommandQueues is created automatically when RHI initializes.
         * @param inQueueType The type of CommandQueue.
         * @return Reference of CommandQueue.
         */
        virtual CommandQueueRef GetCommandQueue(ECommandQueueType inQueueType) = 0;
        /**
         * Create CommandPool for given CommandQueue. The CommandBuffer allocated from this CommandPool **must** only be submitted to given CommandQueue. 
         * @param inCommandQueue 
         * @return Reference of Created CommandPool
         */
        virtual CommandPoolRef CreateCommandPool(CommandQueueRef inCommandQueue) = 0;
    };
}
