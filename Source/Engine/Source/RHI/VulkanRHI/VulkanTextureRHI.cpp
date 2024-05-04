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

#include "VulkanTextureRHI.h"

#include <vulkan/vk_enum_string_helper.h>
#include "Core.h"
#include "RHI/TextureResources.h"
#ifdef INCLUDE_RHI_VULKAN
#include "VkFormatMap.h"
#include "VulkanRHI.h"

namespace Koala::RHI
{
    static Koala::Logger logger("VulkanRHITexture");
    static FORCEINLINE_DEBUGABLE VkImageUsageFlags TextureUsageToVkImageUsageFlags(ETextureUsages usage)
    {
        VkImageUsageFlags flags{0};
        ASSERT(usage != ETextureUsage::UnknownTexture);

        if (usage & ETextureUsage::ShaderResource)
        {
            flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if (usage & ETextureUsage::PresentTexture)
        {
            flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if (usage & ETextureUsage::RenderTarget && usage & ETextureUsage::ColorTexture)
        {
            flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if (
            usage & ETextureUsage::RenderTarget &&
            (usage & ETextureUsage::DepthTexture || usage & ETextureUsage::StencilTexture)
        )
        {
            flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        if (usage & ETextureUsage::CopySrcTexture)
        {
            flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }
        if (usage & ETextureUsage::CopyDstTexture)
        {
            flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }
        if (usage & ETextureUsage::GPUWriteTexture)
        {
            flags |= VK_IMAGE_USAGE_STORAGE_BIT;
        }
        if (usage & ETextureUsage::ShaderResource && usage & ETextureUsage::RenderTarget)
        {
            // This rendertarget will be used in some subsequent subpasses as input.
            flags |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        }
        return flags;
    }
    static FORCEINLINE VkImageLayout TextureUsageToVkImageLayout(ETextureUsages usage)
    {
        if (usage & ETextureUsage::LinearLayout)
            return VK_IMAGE_LAYOUT_PREINITIALIZED;
        else
            return VK_IMAGE_LAYOUT_UNDEFINED;
    }
    static FORCEINLINE VkSampleCountFlagBits NumSamplesToVkSampleCount(int numSamples)
    {
        if (numSamples == 1)
            return VK_SAMPLE_COUNT_1_BIT;
        else if (numSamples == 2)
            return VK_SAMPLE_COUNT_2_BIT;
        else if (numSamples == 4)
            return VK_SAMPLE_COUNT_4_BIT;
        else if (numSamples == 8)
            return VK_SAMPLE_COUNT_8_BIT;
        else if (numSamples == 16)
            return VK_SAMPLE_COUNT_16_BIT;
        else if (numSamples == 32)
            return VK_SAMPLE_COUNT_32_BIT;
        else if (numSamples == 64)
            return VK_SAMPLE_COUNT_64_BIT;
        ASSERTS(0, "You have invalid SampleCount: Vulkan not supported");
        return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
    }
    static FORCEINLINE VkImageType DepthToVkImageType(int depth)
    {
        switch (depth)
        {
        case 1:
            return VK_IMAGE_TYPE_1D;
        case 2:
            return VK_IMAGE_TYPE_2D;
        case 3:
            return VK_IMAGE_TYPE_3D;
        default:
            check(0);
            return VK_IMAGE_TYPE_MAX_ENUM;
        }
    }
    static FORCEINLINE VkComponentSwizzle TextureChannelToVkComponentSwizzle(ETextureChannel channel)
    {
        switch (channel)
        {
        case ETextureChannel::Identity:
            return VK_COMPONENT_SWIZZLE_IDENTITY;
        case ETextureChannel::ChannelR:
            return VK_COMPONENT_SWIZZLE_R;
        case ETextureChannel::ChannelG:
            return VK_COMPONENT_SWIZZLE_G;
        case ETextureChannel::ChannelB:
            return VK_COMPONENT_SWIZZLE_B;
        case ETextureChannel::ChannelA:
            return VK_COMPONENT_SWIZZLE_A;
        case ETextureChannel::One:
            return VK_COMPONENT_SWIZZLE_ONE;
        case ETextureChannel::Zero:
            return VK_COMPONENT_SWIZZLE_ZERO;
        default:
            check(false);
            return VK_COMPONENT_SWIZZLE_MAX_ENUM;
        }

    }
    static FORCEINLINE_DEBUGABLE VmaMemoryUsage TextureUsageToVmaMemoryUsage(ETextureUsages inUsage)
    {
        if (inUsage & ETextureUsage::GPUOnlyTexture || inUsage & ETextureUsage::PresentTexture)
            return VMA_MEMORY_USAGE_GPU_ONLY;
        if (
            inUsage & ETextureUsage::CPUReadTexture &&
            !(inUsage & ETextureUsage::CPUWriteTexture) &&
            !(inUsage & ETextureUsage::PresentTexture))
            return VMA_MEMORY_USAGE_GPU_TO_CPU;
        if (
            inUsage & ETextureUsage::CPUWriteTexture &&
            !(inUsage & ETextureUsage::CPUReadTexture) &&
            !(inUsage & ETextureUsage::PresentTexture))
            return VMA_MEMORY_USAGE_CPU_TO_GPU;
        
        if (
            (
                inUsage & ETextureUsage::RenderTarget ||
                inUsage & ETextureUsage::ShaderResource ||
                inUsage & ETextureUsage::GPUWriteTexture
            ) && (!(inUsage & ETextureUsage::CPUReadTexture) && (!(inUsage & ETextureUsage::CPUWriteTexture))))
            return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        return VMA_MEMORY_USAGE_AUTO;
    }
    static FORCEINLINE_DEBUGABLE VkImageAspectFlagBits TextureUsagesToVkImageAspectFlagBits(ETextureUsages usage)
    {
        if (usage & ETextureUsage::ColorTexture)
            return VK_IMAGE_ASPECT_COLOR_BIT;
        if (usage & ETextureUsage::DepthTexture)
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        if (usage & ETextureUsage::StencilTexture)
            return VK_IMAGE_ASPECT_STENCIL_BIT;
        ensure(false, "You must specify one of Color/Depth/Stencil in usages!");
        return VK_IMAGE_ASPECT_NONE;
    }

    TextureRHIRef VulkanTextureInterface::CreateTexture(const char* debugName, const RHITextureCreateInfo& info)
    {
        check(info.depth <= 3);
        VkImageCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.format = PixelFormatToVkFormat(info.pixelFormat);
        createInfo.imageType = DepthToVkImageType(info.depth);
        createInfo.extent.width = info.size.x;
        createInfo.extent.height = info.size.y;
        createInfo.extent.depth = info.depth;
        createInfo.mipLevels = info.numMips;
        createInfo.samples = NumSamplesToVkSampleCount(info.numSamples);
        createInfo.sharingMode = info.usage & (uint32_t)NonExclusiveAccessTexture ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
        createInfo.initialLayout = TextureUsageToVkImageLayout(info.usage);
        createInfo.usage = TextureUsageToVkImageUsageFlags(info.usage);
        createInfo.arrayLayers = info.numTextureArray;
        createInfo.tiling = (info.usage & ETextureUsage::LinearLayout) ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
        
        VmaAllocationCreateInfo vmaAllocationCreateInfo{};
        vmaAllocationCreateInfo.usage = TextureUsageToVmaMemoryUsage(info.usage);
        // We only interested in GPU Memory.
        vmaAllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        auto textureRHI = std::make_shared<VulkanTextureRHI>();
        
        textureRHI->cachedTextureCreateInfo = info;
        textureRHI->cachedTextureInfo.format = createInfo.format;
        textureRHI->cachedTextureInfo.samples = createInfo.samples;
        textureRHI->cachedTextureInfo.usage = createInfo.usage;
        textureRHI->cachedTextureInfo.imageType = createInfo.imageType;
        textureRHI->cachedTextureInfo.initialLayout = createInfo.initialLayout;

        VK_CHECK_RESULT_SUCCESS(vmaCreateImage(VulkanRHI::GetVkRuntime()->vmaAllocator, &createInfo, &vmaAllocationCreateInfo, &textureRHI->image, &textureRHI->vmaAllocation, nullptr))

#if RHI_ENABLE_GPU_MARKER
        SetTextureDebugName(*textureRHI, debugName);
        textureRHI->cachedTextureInfo.debugName = debugName;
#endif
        return textureRHI;
    }
    
    TextureViewRef VulkanTextureInterface::CreateTextureView(TextureRHIRef inTexture, bool bUseSwizzle)
    {
        auto view = std::make_shared<VulkanTextureView>();
        auto vulkanTextureRHI = static_cast<VulkanTextureRHI*>(inTexture->GetPlatformNativePointer());

        CreateImageView(view->imageView, *vulkanTextureRHI, bUseSwizzle, TextureUsagesToVkImageAspectFlagBits(vulkanTextureRHI->GetTextureUsage()));
#if RHI_ENABLE_GPU_MARKER
        SetTextureViewDebugName(*view, vulkanTextureRHI->cachedTextureInfo.debugName.c_str());
#endif
        return view;
    }

    void VulkanTextureInterface::CreateImageView(VkImageView& outImageView, const VulkanTextureRHI& image, bool bUseSwizzle, VkImageAspectFlags vkImageAspectFlags) {
        VkImageViewCreateInfo vkImageViewCreateInfo{};
        vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        vkImageViewCreateInfo.image = image.image;

        if (image.cachedTextureCreateInfo.depth == 1)
            vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_1D;
        else if (image.cachedTextureCreateInfo.depth == 2)
            vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        else if (image.cachedTextureCreateInfo.depth == 3)
            vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
        else
            check(false);

        vkImageViewCreateInfo.format = image.cachedTextureInfo.format;

        if (!bUseSwizzle)
        {
            vkImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            vkImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            vkImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            vkImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        }
        else
        {
            vkImageViewCreateInfo.components.r = TextureChannelToVkComponentSwizzle(image.cachedTextureCreateInfo.channelSwizzleInfo.r);
            vkImageViewCreateInfo.components.g = TextureChannelToVkComponentSwizzle(image.cachedTextureCreateInfo.channelSwizzleInfo.g);
            vkImageViewCreateInfo.components.b = TextureChannelToVkComponentSwizzle(image.cachedTextureCreateInfo.channelSwizzleInfo.b);
            vkImageViewCreateInfo.components.a = TextureChannelToVkComponentSwizzle(image.cachedTextureCreateInfo.channelSwizzleInfo.a);
        }
        
        vkImageViewCreateInfo.subresourceRange.aspectMask = vkImageAspectFlags; //  COLOR/DEPTH/STENCIL, etc
        vkImageViewCreateInfo.subresourceRange.baseMipLevel = image.cachedTextureCreateInfo.beginMipLevel;
        vkImageViewCreateInfo.subresourceRange.levelCount = image.cachedTextureCreateInfo.numMips;
        vkImageViewCreateInfo.subresourceRange.baseArrayLayer = image.cachedTextureCreateInfo.beginTextureArrayIndex;
        vkImageViewCreateInfo.subresourceRange.layerCount = image.cachedTextureCreateInfo.numTextureArray;


        VK_CHECK_RESULT_SUCCESS(vkCreateImageView(VulkanRHI::GetVkRuntime()->device, &vkImageViewCreateInfo, nullptr, &outImageView))
    }
    void VulkanTextureInterface::ReleaseTexture(VulkanTextureRHI& inTexture)
    {
        vmaDestroyImage(VulkanRHI::GetVkRuntime()->vmaAllocator, inTexture.image, inTexture.vmaAllocation);
    }
    void VulkanTextureInterface::ReleaseTextureView(VulkanTextureView& inTextureView)
    {
        vkDestroyImageView(VulkanRHI::GetVkRuntime()->device, inTextureView.imageView, nullptr);
    }
    VulkanTextureRHI::~VulkanTextureRHI()
    {
        VulkanTextureInterface::Get().ReleaseTexture(*this);
    }
    VulkanTextureView::~VulkanTextureView()
    {
        VulkanTextureInterface::Get().ReleaseTextureView(*this);
    }
#if RHI_ENABLE_GPU_MARKER
    void VulkanTextureInterface::SetTextureDebugName(const VulkanTextureRHI &inVulkanTextureRHI, const char *inLabel)
    {
        VkDebugUtilsObjectNameInfoEXT vkDebugUtilsObjectNameInfoExt{.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
        vkDebugUtilsObjectNameInfoExt.objectType = VK_OBJECT_TYPE_IMAGE;
        vkDebugUtilsObjectNameInfoExt.objectHandle = reinterpret_cast<uint64_t>(inVulkanTextureRHI.image);
        vkDebugUtilsObjectNameInfoExt.pObjectName = inLabel;

        vkSetDebugUtilsObjectNameEXT(VulkanRHI::GetVkRuntime()->device, &vkDebugUtilsObjectNameInfoExt);
    }
    void VulkanTextureInterface::SetTextureViewDebugName(const VulkanTextureView& inVulkanTextureViewRHI, const char *inLabel)
    {
        VkDebugUtilsObjectNameInfoEXT vkDebugUtilsObjectNameInfoExt{.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
        vkDebugUtilsObjectNameInfoExt.objectType = VK_OBJECT_TYPE_IMAGE_VIEW;
        vkDebugUtilsObjectNameInfoExt.objectHandle = reinterpret_cast<uint64_t>(inVulkanTextureViewRHI.imageView);
        vkDebugUtilsObjectNameInfoExt.pObjectName = inLabel;

        vkSetDebugUtilsObjectNameEXT(VulkanRHI::GetVkRuntime()->device, &vkDebugUtilsObjectNameInfoExt);
    }
#endif
}
#endif