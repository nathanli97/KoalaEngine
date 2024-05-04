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
#include <memory>

#include "Definations.h"

namespace Koala::RHI
{
    enum EBufferUsage
    {
        UnknownBuffer = 0,
        VertexBuffer = 1 << 0,
        IndexBuffer = 1 << 1,
        IndirectBuffer = 1 << 2,
        UniformBuffer = 1 << 3,
        
        CPUWriteBuffer = 1 << 4,
        CPUReadBuffer = 1 << 5,
        GPUWriteBuffer = 1 << 6, // 'Storage Buffer' in VK. GPU can write data into this buffer. Access to this type of buffer may be slower than read-only buffer.
        GPUOnlyBuffer = 1 << 7,

        DynamicOffsetBuffer = 1 << 8, // Can provide offset dynamically.
        TexelBuffer = 1 << 9, // Treat buffer data as image. i.e. we can perform sample operations on this buffer.

        BufferMemoryMapSequentialWrite, // You need map the buffer (On GPU Memory) to HOST (CPU) Memory, and want to write them sequentially.  (WARNING: sequential write only! DO NOT READ OR WRITE RANDOMLY!)
        BufferMemoryMapRandomAccess,    // You need map the buffer (On GPU Memory) to HOST (CPU) Memory, and want to read&write them randomly. (Random access)
        NonExclusiveAccessBuffer = 1 << 31, // This buffer can access from multiple device queues. Slower than exclusive mode.
    };
    typedef uint32_t EBufferUsages;

    struct BufferCopyInfo
    {
        size_t srcOffset;
        size_t dstOffset;
        // If size == 0, will copy whole srcBuffer to dstBuffer, the copySize will take min(srcBufferSize, dstBufferSize)
        size_t size;

        BufferCopyInfo() = default;
    };
    
    struct RHIBufferCreateInfo
    {
        uint32_t size;
        EBufferUsages usage;
    };

    class BufferRHI
    {
    public:
        virtual ~BufferRHI() = default;
        virtual size_t GetPlatformSize() = 0;
        FORCEINLINE_DEBUGABLE const RHIBufferCreateInfo& GetCachedCreateInfo() const {return cachedBufferCreateInfo;}

        const void* GetPlatformNativePointer() const {return this;}
        void* GetPlatformNativePointer() {return this;}
    protected:
        RHIBufferCreateInfo cachedBufferCreateInfo{};
    };

    typedef std::shared_ptr<BufferRHI>  BufferRHIRef;
}
