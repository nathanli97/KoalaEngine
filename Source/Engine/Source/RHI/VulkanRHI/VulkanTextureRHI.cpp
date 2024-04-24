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

#include "Definations.h"
#include "Core/Logger.h"
#include "Renderer/PixelFormat.h"
#include "RHI/TextureRHI.h"
#ifdef INCLUDE_RHI_VULKAN
#include "VulkanRHI.h"

namespace Koala::RHI
{
    static Koala::Logger logger("RHITexture");
    static VkFormat PixelFormatToVkFormat(EPixelFormat pixelFormat)
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
    static VkImageUsageFlags TextureUsageToVkImageUsageFlags(ETextureUsages usage)
    {
        VkImageUsageFlags flags{0};
        ASSERT(usage != ETextureUsage::Unknown);

        if (usage & ETextureUsage::RenderResource)
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
        return flags;
    }
    static VkSampleCountFlagBits NumSamplesToVkSampleCount(int numSamples)
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
    TextureRHIRef VulkanRHI::CreateTexture(const char* debugName, const RHITextureCreateInfo& info)
    {
        VkImageCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.format = PixelFormatToVkFormat(info.pixelFormat);
        createInfo.extent.width = info.size.x;
        createInfo.extent.height = info.size.y;
        createInfo.extent.depth = info.depth;
        createInfo.usage = TextureUsageToVkImageUsageFlags(info.usage);
        createInfo.mipLevels = info.numMips;
        createInfo.samples = NumSamplesToVkSampleCount(info.numSamples);

        VmaAllocationCreateInfo vmaAllocationCreateInfo{};
        
        if (info.usage & GPUOnly)
        {
            vmaAllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

            if (info.usage & CPURead)
                ASSERTS(0, "Invalid TextureUsage");
        }

        vmaAllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        
        VulkanTextureRHI *textureRHI{new VulkanTextureRHI};
        
        VkResult result = vmaCreateImage(vk.vma_allocator, &createInfo, &vmaAllocationCreateInfo, &textureRHI->imageRef, &textureRHI->allocRef, nullptr);

        if (result != VK_SUCCESS)
        {
            logger.error("Failed to allocate texture with {}", string_VkResult(result));
            return nullptr;
        }
        
        return textureRHI;
    }

}
#endif