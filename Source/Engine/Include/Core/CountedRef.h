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

namespace Koala
{
    template<typename T>
    class ICountedRef
    {
    public:
        NODISCARD FORCEINLINE bool IsRef() const {return count != UINT64_MAX;}
        NODISCARD FORCEINLINE ICountedRef& GetRef() const
        {
            check(IsRef());
            return *AsRef();
        }
        
        ICountedRef(T* inTarget):
            target(static_cast<void*>(const_cast<std::remove_cv_t<T>*>(inTarget))),
            count(1)
        {}
        ICountedRef(const ICountedRef& other)
        {
            if (other.IsRef())
            {
                
            }
        }
    private:
        NODISCARD FORCEINLINE ICountedRef& AsRef() const { return static_cast<ICountedRef&>(*target); }
        NODISCARD FORCEINLINE const T* AsTarget() const { return static_cast<const T*>(target); }
        NODISCARD FORCEINLINE T* AsTarget() { return static_cast<T*>(target); }
        
        // count == UINT64_MAX means this is target itself.
        size_t count{UINT64_MAX};
        void* target{nullptr};
    };
}
