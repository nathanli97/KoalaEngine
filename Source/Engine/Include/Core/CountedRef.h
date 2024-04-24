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

#include "Definations.h"

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
        CountedRef(CountedRef& other) noexcept
        {
            InitAsReference(&other);
        }
        CountedRef& operator=(const CountedRef& other) noexcept
        {
            if (&other != this)
            {
                InitAsReference(&other);
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
                InitAsReference();
                _storage.obj.object = other._storage.obj.object;
                _storage.obj.refCount = other._storage.obj.refCount;
            }
            return *this;
        }
        CountedRef(ElementType* inElement)
        {
            InitAsObject(inElement);
        }
        CountedRef& operator=(ElementType* inElement)
        {
            InitAsObject(inElement);
            return *this;
        }


        CountedRef()
        {
            InitAsReference();
        }
        ~CountedRef()
        {
            DecrementRefCount();
        }

        
        bool IsValid() const {return (!IsRef() || _storage.ref.refObject != nullptr);}
        operator bool() const {return IsValid();}
        bool operator==(const CountedRef& rhs) const
        {
            if (
                !IsValid() && rhs.IsValid() ||
                IsValid() && !rhs.IsValid()
                )
                return false;
            if (!IsValid() && !rhs.IsValid())
                return true;

            return GetRef() == rhs.GetRef();
        }
        bool operator!=(const CountedRef& rhs) const
        {
            return !operator==(rhs);
        }
    private:
        void InitAsReference(const CountedRef* other = nullptr)
        {
            if (IsValid()) DecrementRefCount();
            _storage.ref.pad = UINT64_MAX;
            if (other != nullptr)
            {
                _storage.ref.refObject = other->GetRef();
                IncrementRefCount();
            }
            else
            {
                _storage.ref.refObject = nullptr;
            }
        }
        void InitAsObject(ElementType *inElement = nullptr)
        {
            if (IsValid()) DecrementRefCount();
            _storage.obj.object = inElement;
            _storage.obj.refCount = inElement ? 1 : 0;
        }
        

        
        union _Storage 
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

            _Storage()
            {
                memset(this, 0, sizeof(_Storage));
                ref.pad = UINT64_MAX;
            }
        } _storage;

        
        // Here we only return non-const reference pointer -- We may need to call IncrementRefCount() or DecrementRefCount() to modify them.
        CountedRef* GetRef() const
        {
            if(IsRef())
                return GetRef_UnSafe();
            else
            {
                // This is object itself!
                return const_cast<CountedRef*>(static_cast<const CountedRef*>(this));
            }
        }
        CountedRef* GetRef_UnSafe() const {return _storage.ref.refObject;}

        
        void IncrementRefCount() {++(GetRef()->_storage.obj.refCount);}
        void DecrementRefCount()
        {
            --(GetRef()->_storage.obj.refCount);
            if (!IsRef() && _storage.obj.refCount == 0)
            {
                delete _storage.obj.object;
            }
        }
        bool IsRef() const {return _storage.ref.pad == UINT64_MAX;}
    };
}
