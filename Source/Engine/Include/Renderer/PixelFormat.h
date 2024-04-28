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
#include <cstdint>

namespace Koala {
    enum EPixelFormat {
        PF_R32G32B32A32_Float,
        PF_R8G8B8A8,
        // Single channel
        PF_R8,
        // Compressed format, currectly not supported
        PF_DXT1,
        PF_DXT3,
        PF_DXT5,
        PF_BC5,
        PF_MAX
    };

    enum ETextureUsage
    {
        Unknown = 0,
        CPURead = 1 << 0,
        Present = 1 << 1,
        CopySrc = 1 << 2,
        CopyDst = 1 << 3,

        // Can be used as RenderTarget
        RenderTarget = 1 << 4,
        // Can be sampled on GPU shaders
        ShaderResource = 1 << 5,

        // This texture should only be used in GPU (After creation)
        GPUOnly = 1 << 6,
    };
    typedef uint32_t ETextureUsages;
}
