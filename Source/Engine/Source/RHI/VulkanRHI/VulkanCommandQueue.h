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
#include <unordered_map>

#include "Definations.h"
#include "RHI/CommandBufferResources.h"
#include "Runtime.h"
#include "Core/Check.h"
#include "Core/SingletonInterface.h"
#ifdef INCLUDE_RHI_VULKAN
namespace Koala::RHI
{
    struct VulkanCommandQueue final: public RHICommandQueue
    {
        uint32_t queueFamilyIndex;
        VkQueue vkQueue;
    };

    class VulkanCommandQueueRegister final: public ISingleton
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(VulkanCommandQueueRegister)
        VulkanCommandQueue* GetQueue(ECommandQueueType inType)
        {
            check(vkQueueMap.contains(inType));
            return &vkQueueMap.at(inType);
        }
        void AddUninitializedCommandQueue() {}
        template<typename... Type>
        void AddUninitializedCommandQueue(ECommandQueueType inType, Type... inTypes)    
        {
            check(!vkQueueMap.contains(inType));
            vkQueueMap.emplace(inType, VulkanCommandQueue{});
            AddUninitializedCommandQueue(inTypes...);
        }
    private:
        std::unordered_map<ECommandQueueType, VulkanCommandQueue> vkQueueMap; 
    };
}
#endif