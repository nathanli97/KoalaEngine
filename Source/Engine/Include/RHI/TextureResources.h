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
#include "Math/Rect.h"
#include "Renderer/PixelFormat.h"

namespace Koala::RHI
{
    class ITextureInterface;
    enum ETextureChannel
    {
        ChannelR,
        ChannelG,
        ChannelB,
        ChannelA,
        Identity,
        One,
        Zero,
        TextureChannelMax
    };

    /**
     * @brief The usage flag for texture.
     * @details This flag descriptures how the program use this texture, the underlying RHI code \
     *          can take this flag to optimize how the texture is created and used, to maximum runtime performance.
     */
    enum ETextureUsage
    {
        // Invalid texture. Do not use this flag to create texture.
        UnknownTexture = 0,
        // This texture can be used to present to surface (framebuffer).
        PresentTexture = 1 << 0,
        // This texture can be used as source in transfer command (COPY SRC).
        CopySrcTexture = 1 << 1,
        // This texture can be used as destination in transfer command (COPY DST).
        CopyDstTexture = 1 << 2,

        // Can be used as RenderTarget.
        RenderTarget = 1 << 3,
        // Can be used as ShaderResource.
        ShaderResource = 1 << 4,

        // This texture can have RGB(A) values.
        ColorTexture = 1 << 5,
        // This texture should treat as depth-only texture. NOTE `DepthTexture` and `StencilTexture` can be used together, In such case texture format maybe like D24S8.
        DepthTexture = 1 << 6,
        // This texture should treat as stencil-only texture. NOTE `DepthTexture` and `StencilTexture` can be used together, In such case texture format maybe like D24S8.
        StencilTexture = 1 << 7,

        // AVOID: This texture can be written by CPU. For performance consideration, this flag should avoid.
        CPUWriteTexture = 1 << 8,
        // AVOID: This texture can be written by CPU. For performance consideration, this flag should avoid.
        CPUReadTexture = 1 << 9,
        // This texture can be written by GPU.
        GPUWriteTexture = 1 << 10,
        // RECOMMENDED: This texture can be written by GPU.
        GPUOnlyTexture = 1 << 11,
        
        /**
         * By the default, all textures use optimal layout and use staging buffer for loading.
         * If you don't want use staging buffer for texture loading, then use 'LinearLayout'.
         * This will cause created texture is in linear layout, i.e. can load texture data directly.
         */
        LinearLayout = 1 << 12,
        

        NonExclusiveAccessTexture = 1 << 31, // This image can access from multiple device queues. Slower than exclusive mode.
    };

    typedef uint32_t ETextureUsages;

    NODISCARD FORCEINLINE bool IsValidTextureUsage(ETextureUsages usages)
    {
        if (usages == ETextureUsage::UnknownTexture)
            return false;

        if (usages & ETextureUsage::GPUOnlyTexture)
        {
            if (usages & ETextureUsage::CPUReadTexture || usages & ETextureUsage::CPUWriteTexture)
                return false;
        }

        if (usages & ETextureUsage::ColorTexture && (usages & ETextureUsage::DepthTexture || usages & ETextureUsage::StencilTexture))
        {
            return false;
        }
        
        return true;
    }
    
    struct RHITextureCreateInfo
    {
        EPixelFormat pixelFormat{PF_R8G8B8A8};
        ETextureUsages usage{ETextureUsage::UnknownTexture};
        UInt32Point size{};
        int beginMipLevel{0};
        int numMips{0};
        
        // TODO： Multisample support -- not supported yet
        int numSamples{0};

        int beginTextureArrayIndex{0};
        int numTextureArray{0};
        uint8_t depth{};
        
        struct ChannelSwizzleInfo
        {
            ETextureChannel r = ETextureChannel::Identity;
            ETextureChannel g = ETextureChannel::Identity;
            ETextureChannel b = ETextureChannel::Identity;
            ETextureChannel a = ETextureChannel::Identity;
            NODISCARD bool IsIdentity() const
            {
                return r == ChannelR && g == ChannelB && b == ChannelB && a == ChannelA ||
                    r == ETextureChannel::Identity && g == ETextureChannel::Identity && b == ETextureChannel::Identity && a == ETextureChannel::Identity;
            }
            FORCEINLINE void SetIdentity()
            {
                r = ETextureChannel::Identity;
                g = ETextureChannel::Identity;
                b = ETextureChannel::Identity;
                a = ETextureChannel::Identity;
            }
        } channelSwizzleInfo;
    };

    class TextureRHI
    {
    public:
        friend class ITextureInterface;
        virtual ~TextureRHI() = default;
        virtual size_t GetPlatformSize() = 0;
        
        FORCEINLINE_DEBUGABLE uint8_t GetDepth() const {return cachedTextureCreateInfo.depth;}
        FORCEINLINE_DEBUGABLE uint32_t GetMipNum() const {return cachedTextureCreateInfo.numMips;}
        FORCEINLINE_DEBUGABLE uint32_t GetMipBeginLevel() const {return cachedTextureCreateInfo.beginMipLevel;}
        FORCEINLINE_DEBUGABLE UInt32Point GetExtent() const {return cachedTextureCreateInfo.size;}
        FORCEINLINE_DEBUGABLE ETextureUsages GetTextureUsage() const {return cachedTextureCreateInfo.usage;}
        FORCEINLINE_DEBUGABLE const RHITextureCreateInfo& GetCachedCreateInfo() const {return cachedTextureCreateInfo;}

        const void* GetPlatformNativePointer() const {return this;}
        void* GetPlatformNativePointer() {return this;}

    protected:
        RHITextureCreateInfo cachedTextureCreateInfo{};
    };

    
    typedef std::shared_ptr<TextureRHI>  TextureRHIRef;
    
    class TextureView
    {
    public:
        virtual ~TextureView() = default;
        virtual TextureRHIRef GetTexture() = 0;

        const void* GetPlatformNativePointer() const {return this;}
        void* GetPlatformNativePointer() {return this;}
    };
    
    
    typedef std::shared_ptr<TextureView> TextureViewRef;
}
