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
#include "Definations.h"
#include "Runtime.h"
#include "Core/Check.h"
#include "Renderer/PixelFormat.h"

#ifdef INCLUDE_RHI_VULKAN
#define DEFINE_VK_FORMAT_MAP(engineFormat, vkFormat) case engineFormat: return vkFormat;
namespace Koala::RHI
{
    static FORCEINLINE VkFormat PixelFormatToVkFormat(EPixelFormat pixelFormat)
    {
        ASSERT(pixelFormat < PF_MAX);
        switch (pixelFormat)
        {
            DEFINE_VK_FORMAT_MAP(PF_R8, VK_FORMAT_R8_SNORM)
            DEFINE_VK_FORMAT_MAP(PF_R8G8, VK_FORMAT_R8G8_SNORM)
            DEFINE_VK_FORMAT_MAP(PF_R8G8B8, VK_FORMAT_R8G8B8_SNORM)
            DEFINE_VK_FORMAT_MAP(PF_R8G8B8A8, VK_FORMAT_R8G8B8A8_SNORM)
            DEFINE_VK_FORMAT_MAP(PF_R16, VK_FORMAT_R16_SNORM)
            DEFINE_VK_FORMAT_MAP(PF_R16G16, VK_FORMAT_R16G16_SNORM)
            DEFINE_VK_FORMAT_MAP(PF_R16G16B16, VK_FORMAT_R16G16B16_SNORM)
            DEFINE_VK_FORMAT_MAP(PF_R16G16B16A16, VK_FORMAT_R16G16B16A16_SNORM)
            DEFINE_VK_FORMAT_MAP(PF_R32, VK_FORMAT_R32_SFLOAT)
            DEFINE_VK_FORMAT_MAP(PF_R32G32, VK_FORMAT_R32G32_SFLOAT)
            DEFINE_VK_FORMAT_MAP(PF_R32G32B32, VK_FORMAT_R32G32B32_SFLOAT)
            DEFINE_VK_FORMAT_MAP(PF_R32G32B32A32, VK_FORMAT_R32G32B32A32_SFLOAT)
            DEFINE_VK_FORMAT_MAP(PF_DXT1, VK_FORMAT_BC1_RGBA_SRGB_BLOCK)
            DEFINE_VK_FORMAT_MAP(PF_DXT3, VK_FORMAT_BC2_SRGB_BLOCK)
            DEFINE_VK_FORMAT_MAP(PF_DXT5, VK_FORMAT_BC3_SRGB_BLOCK)
            DEFINE_VK_FORMAT_MAP(PF_BC5, VK_FORMAT_BC5_SNORM_BLOCK)
        default:
            ensure(0, "You have unimplemented PixelFormat in function PixelFormatToVkFormat");
            return VK_FORMAT_MAX_ENUM;
        }
    }
}
#undef DEFINE_VK_FORMAT_MAP
#endif