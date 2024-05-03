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

#include "VulkanBufferRHI.h"

#include <Core.h>
#include <RGBAColor.h>
#include "VulkanRHI.h"
#include "RHI/TextureResources.h"

#ifdef INCLUDE_RHI_VULKAN
namespace Koala::RHI
{
    static FORCEINLINE VkBufferUsageFlags BufferUsageToVkBufferUsageFlags(EBufferUsages usage)
    {
        VkBufferUsageFlags flags{};
        if (usage & EBufferUsage::VertexBuffer)
            flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if (usage & EBufferUsage::IndexBuffer)
            flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (usage & EBufferUsage::IndirectBuffer)
            flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        if (usage & EBufferUsage::UniformBuffer)
        {
            if (flags & EBufferUsage::TexelBuffer)
                flags |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
            else
                flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
        if (usage & EBufferUsage::GPUWriteBuffer)
        {
            if (flags & EBufferUsage::TexelBuffer)
                flags |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
            else
                flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }
        return flags;
    }
    static FORCEINLINE VmaMemoryUsage BufferUsageToVmaMemoryUsage(EBufferUsages inUsage)
    {
        if (inUsage & EBufferUsage::GPUOnlyBuffer)
            return VMA_MEMORY_USAGE_GPU_ONLY;
        if (
            inUsage & EBufferUsage::CPUReadBuffer &&
            !(inUsage & EBufferUsage::CPUWriteBuffer) &&
            inUsage & EBufferUsage::GPUWriteBuffer)
            return VMA_MEMORY_USAGE_GPU_TO_CPU;
        if (
            inUsage & EBufferUsage::CPUWriteBuffer &&
            !(inUsage & EBufferUsage::CPUReadBuffer) &&
            inUsage & EBufferUsage::GPUWriteBuffer)
            return VMA_MEMORY_USAGE_CPU_TO_GPU;

        return VMA_MEMORY_USAGE_AUTO;
    }
    BufferRHIRef VulkanBufferInterface::CreateBuffer(const char *debugName, const RHIBufferCreateInfo &info)
    {
        VkBufferCreateInfo vkBufferCreateInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        vkBufferCreateInfo.size = info.size;
        vkBufferCreateInfo.usage = BufferUsageToVkBufferUsageFlags(info.usage);
        vkBufferCreateInfo.sharingMode = (info.usage & EBufferUsage::NonExclusiveAccessBuffer) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo vmaAllocationCreateInfo{};
        vmaAllocationCreateInfo.usage = BufferUsageToVmaMemoryUsage(info.usage);
        
        auto bufferRHI = std::make_shared<VulkanBufferRHI>();
        bufferRHI->cachedBufferCreateInfo = info;

        VK_CHECK_RESULT_SUCCESS(vmaCreateBuffer(vkRuntime.vmaAllocator, &vkBufferCreateInfo, &vmaAllocationCreateInfo, &bufferRHI->buffer, &bufferRHI->vmaAllocation, nullptr));
        return nullptr;
    }

#if RHI_ENABLE_GPU_MARKER
    void VulkanBufferInterface::SetBufferDebugName(const VulkanBufferRHI& inVkBufferRHI, const char *inLabel)
    {
        VkDebugUtilsObjectNameInfoEXT vkDebugUtilsObjectNameInfoExt{.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
        vkDebugUtilsObjectNameInfoExt.objectType = VK_OBJECT_TYPE_BUFFER;
        vkDebugUtilsObjectNameInfoExt.objectHandle = reinterpret_cast<uint64_t>(inVkBufferRHI.buffer);
        vkDebugUtilsObjectNameInfoExt.pObjectName = inLabel;
    }
#endif
}
#endif