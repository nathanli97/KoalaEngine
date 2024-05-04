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
#include "AllocatorBase.h"
#include <unordered_map>
#include <set>

namespace Koala {
    class MemoryPool;
    
    // A simple, pooled memory manager for Koala.
    class MemoryAllocator: IAllocatorBase {
    public:
        // template <>
        // using Get = ISingleton::Get<MemoryAllocator>;
        static MemoryAllocator& Get()
        {
            return IAllocatorBase::Get<MemoryAllocator>();
        }
        inline void * Malloc(size_t inSize) override {
            return ::malloc(inSize);
        }
        inline void Free(void *inPtr) override {
            ::free(inPtr);
        }
        void CreatePool(size_t inElementSize, size_t inMaxElementNumInPool);

        // This function will try to match the input size ('inSize') to some pool.
        // If matched successfully, it will allocate memory from that pool.
        // If matched failed, it will just use Malloc() to allocate memory.
        // Note actual allocated memory may larger than required size.
        void *MallocPooled(size_t inSize);

        // This function will try to allocate memory by given type T.
        // Note this function will not to call constructor function if type is class/struct.
        // You should handle the construct manually.
        template <typename T>
        T *MallocPooledTyped() { return static_cast<T*>(MallocPooled(sizeof(T))); }
        
        // This function will free a pooled memory.
        void FreePooled(void *inPtr);

    private:
        std::unordered_map<size_t, MemoryPool*> memoryPools;
        std::set<size_t> memoryPoolSizes;
    };

    namespace Memory
    {
        inline void * Malloc(size_t inSize) {
            return MemoryAllocator::Get().Malloc(inSize);
        }
        inline void Free(void *inPtr) {
            MemoryAllocator::Get().Free(inPtr);
        }
        template<typename Type, typename... Args> Type* New(Args... args)
        {
            Type* memory = static_cast<Type*>(Malloc(sizeof(Type)));
            ConstructElement(*memory, std::forward<Args&&>(args)...);
            return memory;
        }
        template<typename Type> void Delete(Type *ptr)
        {
            DestructElement(*ptr);
            Free(ptr);
        }
    }
}
