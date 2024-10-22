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
#include "RHI/Interfaces/TextureRHI.h"
#include "Runtime.h"
#include "RHI/TextureResources.h"


namespace Koala::RHI
{
    class VulkanTextureInterface;
    
    class VulkanTextureRHI: public TextureRHI
    {
    public:
        friend class VulkanTextureInterface;
        VkImage image{};
        VmaAllocation vmaAllocation{};
        virtual ~VulkanTextureRHI() override;
        
        struct
        {
            VkFormat format;
            VkImageType imageType;
            VkSampleCountFlagBits samples;
            VkImageUsageFlags usage;
            VkImageLayout initialLayout;
#if RHI_ENABLE_GPU_MARKER
            std::string debugName;
#endif
        } cachedTextureInfo{};

        // VmaAllocationInfo cachedAllocationInfo{};
        size_t GetPlatformSize() override {return 0;}
    };

    class VulkanTextureView: public TextureView
    {
    public:
        virtual ~VulkanTextureView() override;
        std::shared_ptr<VulkanTextureRHI> linkedTexture{};
        VkImageView imageView{};
        FORCEINLINE_DEBUGABLE TextureRHIRef GetTexture() override {return linkedTexture;}
    };

    class VulkanTextureInterface: public ITextureInterface
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(VulkanTextureInterface)
        
        TextureRHIRef CreateTexture(const char* debugName, const RHITextureCreateInfo& info) override;
        TextureViewRef CreateTextureView(TextureRHIRef inTexture, bool bUseSwizzle) override;
        void CopyTexture() override {}

        void ReleaseTexture(VulkanTextureRHI& inTexture);
        void ReleaseTextureView(VulkanTextureView& inTextureView);
    private:
#if RHI_ENABLE_GPU_MARKER
        void SetTextureDebugName(const VulkanTextureRHI& inVulkanTextureRHI, const char *inLabel);
        void SetTextureViewDebugName(const VulkanTextureView& inVulkanTextureViewRHI, const char *inLabel);

#endif
        void CreateImageView(VkImageView &outImageView, const VulkanTextureRHI& image, bool bUseSwizzle, VkImageAspectFlags vkImageAspectFlags);
    };
}
#endif