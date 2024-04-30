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
#include "RHI/Interfaces/TextureRHI.h"

#ifdef INCLUDE_RHI_VULKAN
#include "Runtime.h"
#include "RHI/TextureResources.h"


namespace Koala::RHI
{
    struct VulkanTextureRHI: public TextureRHI
    {
        VkImage image;
        VmaAllocation vmaAllocation;
        
        RHITextureCreateInfo cachedTextureCreateInfo;

        struct
        {
            VkFormat format;
            VkImageType imageType;
            VkSampleCountFlagBits samples;
            VkImageUsageFlags usage;
        } cachedTextureInfo;

        FORCEINLINE_DEBUGABLE size_t GetSize() override
        {
            return 0;
        }
        FORCEINLINE_DEBUGABLE uint8_t GetDepth() override
        {
            return cachedTextureCreateInfo.depth;
        }
        FORCEINLINE_DEBUGABLE uint32_t GetMipNum() override
        {
            return cachedTextureCreateInfo.numMips;
        }
        FORCEINLINE_DEBUGABLE uint32_t GetMipBeginLevel() override
        {
            return cachedTextureCreateInfo.beginMipLevel;
        }
        FORCEINLINE_DEBUGABLE UInt32Point GetExtent() override
        {
            return cachedTextureCreateInfo.size;
        }
        FORCEINLINE_DEBUGABLE ETextureUsages GetTextureUsage() override
        {
            return cachedTextureCreateInfo.usage;
        }
    };

    struct VulkanTextureView: public TextureView
    {
        std::shared_ptr<VulkanTextureRHI> linkedTexture;
        ETextureViewType viewType;
        VkImageView imageView;
        FORCEINLINE_DEBUGABLE TextureRHIRef GetTexture() override {return linkedTexture;}
    };

    class VulkanTextureInterface: public ITextureInterface
    {
    public:
        VulkanTextureInterface(VulkanRuntime& inVkRuntime):vkRuntime(inVkRuntime) {}
        TextureRHIRef CreateTexture(const char* debugName, const RHITextureCreateInfo& info) override;
        TextureViewRef CreateTextureView(TextureRHIRef inTexture, ETextureViewType inViewType, bool bUseSwizzle) override;
    private:
        void CreateImageView(VkImageView &outImageView, const VulkanTextureRHI& image, bool bUseSwizzle, VkImageAspectFlags vkImageAspectFlags);
        VulkanRuntime &vkRuntime;
    };
}
#endif