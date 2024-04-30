// Copyright 2023 Li Xingru
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the “Software”), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#pragma once
#include <bitset>
#include <forward_list>

#include "Memory/Allocator.h"

namespace Koala
{
    class MemoryAllocator;
}

namespace Koala::Memory
{
    /**
     * Templated, memory aligned version of Fixed-Size Memory Pool Manager.
     * NOTE This version of mem-pool does not thread-safe, and not allocate inline memory when initializing.
     * FIXME: The element didn't destruct when releasing to pool.
     * Original version of this pool is designed & implemented by Ben Kenwright, School of Computer Science of Newcastle University. Reference: https://arxiv.org/pdf/2210.16471
     * @tparam Type The type you want to manage with memory pool.
     * @tparam NumBlocks The maximum blocks the memory pool can allocate.
     * @tparam RequiredAlignment Default to 1. The required memory alignment, in bytes.
     * @tparam bAutoInitialize Default to true. If true, initialize the pool automatically when construct a new TMemeoryPool.
     * @tparam bAllowAllocFail Default to true. If false, stop the engine running and generate dump when OOM.
     * @tparam BlockSize Auto computed unaligned size of each block. Don't override this template argument.
     * @tparam PadSize Auto computed aligned pad size. Don't override this template argument.
     * @tparam TotalSizeAligned Auto computed aligned size. Don't override this template argument.
     */
    template <
        typename Type, uint32_t NumBlocks, uint16_t RequiredAlignment = 1, bool bAutoInitialize = true, bool bAllowAllocFail = true,
        uint32_t BlockSize = sizeof(Type),
        uint16_t PadSize = CalculateAlignedPadSize(RequiredAlignment, NumBlocks * BlockSize),
        uint64_t TotalSizeAligned = CalculateAlignedSize(RequiredAlignment, NumBlocks * BlockSize)
    >
    class TMemoryPool
    {
    public:
        static_assert(NumBlocks < UINT32_MAX, "Too much blocks!");

        // Memory Pool can not be copied, or moved.
        TMemoryPool(const TMemoryPool&) = delete;
        TMemoryPool& operator=(const TMemoryPool&) = delete;
        TMemoryPool(TMemoryPool&&) = delete;
        TMemoryPool& operator=(TMemoryPool&&) = delete;
        
        TMemoryPool()
        {
            if constexpr (bAutoInitialize)
            {
                InitializePool();
            }
        }
        ~TMemoryPool()
        {
            DestroyMemoryInternal();
        }
        uint32_t GetAvailableBlockNum() const
        {
            return numFreeBlocks;
        }
        constexpr static uint32_t GetTotalBlockNum()
        {
            return NumBlocks;
        }
        constexpr static uint32_t GetBlockSize()
        {
            return BlockSize;
        }
        constexpr static size_t GetTotalSize()
        {
            return (size_t)NumBlocks * BlockSize;
        }
        
        FORCEINLINE_DEBUGABLE void *Malloc()
        {
            ASSERT(memStart);
            if (numInitializedBlocks < NumBlocks)
            {
                // We have pending uninitialized blocks remaining.
                auto ptr = static_cast<uint32_t*>(AddressFromIndex(numInitializedBlocks));
                // Initialize new unused blocks as linked-list chain by storing the index of next unused block.
                *ptr = ++numInitializedBlocks;
            }

            void *allocatedMemory = nullptr;

            if (numFreeBlocks > 0)
            {
                allocatedMemory = static_cast<void*>(nextFreeBlockPtr);
                --numFreeBlocks;

                // Do we have unallocated block(s)?
                if (numFreeBlocks != 0)
                {
                    nextFreeBlockPtr = AddressFromIndex(*(reinterpret_cast<uint32_t *>(nextFreeBlockPtr)));
                }
                else
                {
                    // We have allocated all memory blocks.
                    // Reset the next pointer to NULL.
                    nextFreeBlockPtr = nullptr;
                }
            }

            if constexpr (!bAllowAllocFail)
            {
                if (!allocatedMemory)
                {
                    // OOM!
                    // TODO: Replace this by OOM handing logic.
                    std::abort();
                }
            }
            return allocatedMemory;
        }
        FORCEINLINE_DEBUGABLE void Free(void *ptr)
        {
            ASSERT(memStart);
            if (nextFreeBlockPtr != nullptr)
            {
                *(static_cast<uint32_t *>(ptr)) = IndexFromAddress(nextFreeBlockPtr);
            }
            else
            {
                // This is end of free list.
                *(static_cast<uint32_t *>(ptr)) = NumBlocks;
            }

            nextFreeBlockPtr = static_cast<uint8_t*>(ptr);
            ++numFreeBlocks;
        }

        Type* Allocate()
        {
            return static_cast<Type *>(Malloc());
        }

        void Release(const Type* inPtr)
        {
            using NonCVType = std::remove_cv_t<Type>;
            Type* ptr = const_cast<NonCVType*>(inPtr);
            Free(ptr);
        }

        // Destroy this pool.
        // This will make this pool unusable until user initialize again.
        FORCEINLINE_DEBUGABLE void DestroyPool()
        {
            DestroyMemoryInternal();
        }
        // Initialize pool.
        FORCEINLINE_DEBUGABLE void InitializePool()
        {
            if (memStart)
                return;
            memStart = static_cast<uint8_t *>(MemoryAllocator::Get().Malloc(TotalSizeAligned));
            
            if constexpr (PadSize != 0)
            {
                memStart += PadSize;
            }

            numFreeBlocks = NumBlocks;
            numInitializedBlocks = 0;
            nextFreeBlockPtr = memStart;
        }
    private:
        FORCEINLINE void DestroyMemoryInternal()
        {
            if (!memStart)
                return;
            if constexpr (PadSize != 0)
            {
                memStart -= PadSize;
            }
            MemoryAllocator::Get().Free(memStart);
            memStart = nullptr;
        }
        FORCEINLINE uint8_t* AddressFromIndex(uint32_t index) const
        {
            return memStart + (index * BlockSize);
        }
        FORCEINLINE uint32_t IndexFromAddress(const uint8_t* ptr) const
        {
            return static_cast<uint32_t>((ptr - memStart) / BlockSize);
        }
        uint32_t numFreeBlocks{NumBlocks};
        uint32_t numInitializedBlocks{0};
        uint8_t* memStart{nullptr};
        uint8_t* nextFreeBlockPtr{nullptr};
    };
}
