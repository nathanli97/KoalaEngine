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

#ifdef INCLUDE_RHI_VULKAN
#include "RHI/Interfaces/BufferRHI.h"
#include "Runtime.h"

namespace Koala
{
    struct RGBAColor;
}

namespace Koala::RHI
{
    class VulkanBufferInterface;
    
    class VulkanBufferRHI: public BufferRHI
    {
    public:
        friend class VulkanBufferInterface;
        size_t GetPlatformSize() override {return 0;}
        VmaAllocation vmaAllocation{};
        VkBuffer buffer{};
    };
    class VulkanBufferInterface: public IBufferInterface
    {
    public:
        VulkanBufferInterface(VulkanRuntime& inRuntime): vkRuntime(inRuntime) {}
        BufferRHIRef CreateBuffer(const char *debugName, const RHIBufferCreateInfo &info) override;
        void CopyBuffer(CommandBufferRef inCommandBuffer, const char *inDebugName, BufferRHIRef inSrcBuffer,
            BufferRHIRef inDstBuffer, const BufferCopyInfo &info) override {}

    private:
#if RHI_ENABLE_GPU_MARKER
        void SetBufferDebugName(const VulkanBufferRHI& inVkBufferRHI, const char *inLabel);
#endif
        VulkanRuntime &vkRuntime;
    };
}

#endif
