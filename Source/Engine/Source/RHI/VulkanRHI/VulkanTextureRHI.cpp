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
#include "Renderer/PixelFormat.h"
#include "RHI/TextureResources.h"
#include "Memory/Allocator.h"
#ifdef INCLUDE_RHI_VULKAN
#include "VulkanRHI.h"

namespace Koala::RHI
{
    static Koala::Logger logger("VulkanRHITexture");
    static FORCEINLINE VkFormat PixelFormatToVkFormat(EPixelFormat pixelFormat)
    {
        ASSERT(pixelFormat < PF_MAX);
        switch (pixelFormat)
        {
        case PF_R8G8B8A8:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case PF_R32G32B32A32_Float:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case PF_R8:
            return VK_FORMAT_R8_SINT;
        case PF_DXT1:
            return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
        case PF_DXT3:
            return VK_FORMAT_BC2_SRGB_BLOCK;
        case PF_DXT5:
            return VK_FORMAT_BC3_SRGB_BLOCK;
        case PF_BC5:
            return VK_FORMAT_BC5_SNORM_BLOCK;
        default:
            ASSERTS(0, "You have unimplemented PixelFormat in function PixelFormatToVkFormat");
            return VK_FORMAT_MAX_ENUM;
        }
    }
    static FORCEINLINE VkImageUsageFlags TextureUsageToVkImageUsageFlags(ETextureUsages usage)
    {
        VkImageUsageFlags flags{0};
        ASSERT(usage != ETextureUsage::Unknown);

        if (usage & ETextureUsage::ShaderResource)
        {
            flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if (usage & ETextureUsage::RenderTarget || usage & ETextureUsage::Present)
        {
            flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if (usage & ETextureUsage::CopySrc)
        {
            flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }
        if (usage & ETextureUsage::CopyDst)
        {
            flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }
        if (usage & ETextureUsage::CPURead)
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
    static FORCEINLINE VmaMemoryUsage TextureUsageToVmaMemoryUsage(ETextureUsages inUsage)
    {
        if (inUsage & ETextureUsage::GPUOnly || inUsage & ETextureUsage::Present)
            return VMA_MEMORY_USAGE_GPU_ONLY;
        if (
            inUsage & ETextureUsage::CPURead &&
            !(inUsage & ETextureUsage::CPUWrite) &&
            !(inUsage & ETextureUsage::Present)
            )
            return VMA_MEMORY_USAGE_GPU_TO_CPU;
        if (
            inUsage & ETextureUsage::CPUWrite &&
            !(inUsage & ETextureUsage::CPURead) &&
            !(inUsage & ETextureUsage::Present)
            )
            return VMA_MEMORY_USAGE_CPU_TO_GPU;
        
        if (inUsage & ETextureUsage::RenderTarget || inUsage & ETextureUsage::ShaderResource)
            return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

        return VMA_MEMORY_USAGE_AUTO;
    }
    static FORCEINLINE VkImageAspectFlagBits TextureViewTypeToVkImageAspectFlagBits(ETextureViewType viewType)
    {
        switch (viewType)
        {
        case ETextureViewType::Color:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        case ETextureViewType::Depth:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        case ETextureViewType::Stencil:
            return VK_IMAGE_ASPECT_STENCIL_BIT;
        default:
            check(false);
            return VK_IMAGE_ASPECT_NONE;
        }
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
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        createInfo.usage = TextureUsageToVkImageUsageFlags(info.usage);
        createInfo.arrayLayers = info.numTextureArray;
        
        VmaAllocationCreateInfo vmaAllocationCreateInfo{};
        vmaAllocationCreateInfo.usage = TextureUsageToVmaMemoryUsage(info.usage);

        vmaAllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        auto textureRHI = std::make_shared<VulkanTextureRHI>();
        
        textureRHI->cachedTextureCreateInfo = info;
        textureRHI->cachedTextureInfo.format = createInfo.format;
        textureRHI->cachedTextureInfo.samples = createInfo.samples;
        textureRHI->cachedTextureInfo.usage = createInfo.usage;
        textureRHI->cachedTextureInfo.imageType = createInfo.imageType;
        
        VK_CHECK_RESULT_SUCCESS(vmaCreateImage(vkRuntime.vma_allocator, &createInfo, &vmaAllocationCreateInfo, &textureRHI->image, &textureRHI->vmaAllocation, nullptr))

        return textureRHI;
    }
    
    TextureViewRef VulkanTextureInterface::CreateTextureView(TextureRHIRef inTexture, ETextureViewType inViewType, bool bUseSwizzle)
    {
        auto view = std::make_shared<VulkanTextureView>();
        auto vulkanTextureRHI = static_cast<VulkanTextureRHI*>(inTexture->GetPlatformNativePointer());
        view->viewType = inViewType;
        
        

        CreateImageView(view->imageView, *vulkanTextureRHI, bUseSwizzle, TextureViewTypeToVkImageAspectFlagBits(inViewType));
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


        VK_CHECK_RESULT_SUCCESS(vkCreateImageView(vkRuntime.device, &vkImageViewCreateInfo, nullptr, &outImageView))
    }
    
}
#endif