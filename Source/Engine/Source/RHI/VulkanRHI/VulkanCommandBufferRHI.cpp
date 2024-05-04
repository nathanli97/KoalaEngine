// Copyright 2023 Li Xingru
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the “Software”), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "VulkanCommandBufferRHI.h"

#include "VulkanCommandPool.h"
#include "VulkanCommandQueue.h"
#include "VulkanRHI.h"
#ifdef INCLUDE_RHI_VULKAN
namespace Koala::RHI
{
    CommandQueueRef VulkanCommandBufferInterface::GetCommandQueue(ECommandQueueType inQueueType)
    {
        if (!VulkanCommandQueueRegister::Get().HasQueue(inQueueType))
            return nullptr;
        return VulkanCommandQueueRegister::Get().GetQueue(inQueueType);
    }
    
    CommandPoolRef VulkanCommandBufferInterface::CreateCommandPool(CommandQueueRef inCommandQueue)
    {
        std::shared_ptr<VulkanCommandPool> pool;

        auto vkQueue = static_cast<VulkanCommandQueue*>(inCommandQueue->GetPlatformNativePointer());
        VkCommandPoolCreateInfo vkCommandPoolCreateInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        vkCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        // TODO: short-live support? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT 
        vkCommandPoolCreateInfo.queueFamilyIndex = vkQueue->queueFamilyIndex;

        VK_CHECK_RESULT_SUCCESS(vkCreateCommandPool(VulkanRHI::GetVkRuntime()->device, &vkCommandPoolCreateInfo, nullptr, &pool->vkCommandPool))
        return pool;
    }

    void VulkanCommandBufferInterface::ReleaseCommandPool(const VulkanCommandPool& inCommandPool)
    {
        vkDestroyCommandPool(VulkanRHI::GetVkRuntime()->device, inCommandPool.vkCommandPool, nullptr);
    }
    
    VulkanCommandPool::~VulkanCommandPool()
    {
        VulkanCommandBufferInterface::Get().ReleaseCommandPool(*this);
    }
}
#endif
