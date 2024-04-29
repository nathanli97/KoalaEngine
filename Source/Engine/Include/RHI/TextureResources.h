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
#include "Core/CountedPtr.h"
#include "Math/Rect.h"
#include "Renderer/PixelFormat.h"

namespace Koala::RHI
{
    enum ETextureChannel
    {
        ChannelR,
        ChannelG,
        ChannelB,
        ChannelA,
        Identity,
        One,
        Zero
    };
    struct RHITextureCreateInfo
    {
        EPixelFormat pixelFormat{PF_R8G8B8A8};
        ETextureUsages usage{ETextureUsage::Unknown};
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

    struct ITextureRHI
    {
        virtual ~ITextureRHI() = default;
    };
    typedef ICountedPtr<ITextureRHI> TextureRHIRef;
}
