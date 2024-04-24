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

namespace Koala
{
    template<typename T>
    class CountedRef
    {
    public:
        typedef T ElementType;
        size_t GetRefCount() const
        {
            if (IsRef())
                return _storage.ref.refObject->GetRefCount();
            else
                return _storage.obj.refCount;
        }
        CountedRef(const CountedRef& other) noexcept
        {
            if (other.IsRef())
            {
                _storage.ref.refObject = other._storage.ref.refObject;
            }
            other.IncrementRefCount();
        }
        CountedRef& operator=(const CountedRef& other) noexcept
        {
            if (&other != this)
            {
                *this = CountedRef(other);
            }
            return *this;
        }
        explicit CountedRef(CountedRef&& other) noexcept
        {
            _storage.obj.object = other._storage.obj.object;
            _storage.obj.refCount = other._storage.obj.refCount;
        }
        CountedRef& operator=(CountedRef&& other) noexcept
        {
            if (&other != this)
            {
                
            }
            return *this;
        }
        CountedRef(ElementType* inElement)
        {
            _storage.obj.object = inElement;
        }
    private:
        void IncrementRefCount()
        {
        }
        void DecrementRefCount()
        {
        }
        union 
        {
            struct 
            {
                size_t pad{UINT64_MAX};
                CountedRef* refObject{nullptr};
            } ref;

            struct 
            {
                ElementType* object{nullptr};
                size_t refCount{0};
            } obj;
        } _storage;
        bool IsRef() const {return _storage.ref.pad == UINT64_MAX;}
    };
}
