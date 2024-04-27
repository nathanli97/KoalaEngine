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

#include <utility>

namespace Koala
{
    // The type of custom dealloctor.
    template <typename T>
    using DealloctorFunc = void(T*);
    
    // The Counted Pointer.
    // It can hold (and take over of the control of) a normal ptr.
    // Like STL's smart pointer, it will release the object automatically when nowhere is referencing the given object.
    // Assumes the pointer is allocated via C++'s new operator. Custom deallocator is not supported.
    //
    // NOTE If we write:
    //   const ICountedPtr<int> ptr; THIS MEANS th target(int) stored in this pointer is constant,
    //                               not this pointer is constant.
    //   Sample:
    //      const ICountedPtr<int> ptr   <=== EQUALS ===> const int* ptr;
    //      ptr = other_int_ptr;    // OK.     This is *not* a pointer constant.
    //      *ptr = 2;               // NOT OK. This is constant int.
    //
    // NOTE: This is non thread-safe version of CountedPtr
    // TODO: Support thread-safe, delayed release.
    template<typename InType, typename DealloctorFuncType=nullptr_t>
    class ICountedPtr
    {
    public:
        typedef InType Type;
        NODISCARD FORCEINLINE bool IsPtr() const {return count == UINT64_MAX;}
        NODISCARD FORCEINLINE_DEBUGABLE size_t GetCounter()
        {
            if (IsPtr())
            {
                return GetTarget() ? GetTarget()->count : 0;
            }
            else
            {
                return count;
            }
        }
        ~ICountedPtr()
        {
            HandleSelfRelease();
        }
        ICountedPtr(nullptr_t): count(UINT64_MAX),object(nullptr){}
        ICountedPtr& operator=(nullptr_t)
        {
            HandleSelfRelease();
            count = UINT64_MAX;
            object = nullptr;
            return *this;
        }
        ICountedPtr(Type* inObject, DealloctorFuncType inDealloctorFunc = nullptr):
            object(static_cast<void*>(const_cast<std::remove_cv_t<Type>*>(inObject))),
            count(1),
            dealloctorFunc(inDealloctorFunc)
        {
            if (inObject == nullptr)
            {
                // Reset to pointer mode
                count = UINT64_MAX;
                object = nullptr;
            }
        }
        ICountedPtr(): count(UINT64_MAX),object(nullptr) {}
        ICountedPtr(const ICountedPtr& rhs)
        {
            if (rhs.IsPtr())
            {
                SetTarget(rhs.GetTarget());
                if (rhs.GetTarget())
                    rhs.GetTarget()->IncrementCounter();
            }
            else
            {
                SetTarget(&rhs);
                rhs.IncrementCounter();
            }
        }
        ICountedPtr(ICountedPtr&& rhs) noexcept
        {
            MoveToThis(std::forward<ICountedPtr&&>(rhs));
        }
        ICountedPtr& operator=(const ICountedPtr& rhs) noexcept
        {
            if (this != &rhs)
            {
                HandleSelfRelease();
                
                // Call constructor.
                new(this) ICountedPtr(rhs);
            }
            return *this;
        }
        ICountedPtr& operator=(ICountedPtr&& rhs) noexcept
        {
            if (this != &rhs)
            {
                MoveToThis(std::forward<ICountedPtr&&>(rhs));
            }
            return *this;
        }
        
        template<
            typename T, typename Deallocator = nullptr_t,
            std::enable_if_t<std::conjunction_v<std::is_convertible<T, Type>, std::negation<std::is_same<T, Type>>>, bool> = true
        >
        ICountedPtr& operator=(const ICountedPtr<T, Deallocator>& rhs) noexcept
        {
            HandleSelfRelease();
            new(this) ICountedPtr<T,Deallocator>(rhs);
            return *this;
        }
        template<
            typename T, typename Deallocator = nullptr_t,
            std::enable_if_t<std::conjunction_v<std::is_convertible<T, Type>, std::negation<std::is_same<T, Type>>>, bool> = true
        >
        ICountedPtr& operator=(ICountedPtr<T, Deallocator>&& rhs) noexcept
        {
            MoveToThis<ICountedPtr<T, Deallocator>>(std::forward<ICountedPtr<T, Deallocator>>(rhs));
            return *this;
        }
        template<
            typename T, typename Deallocator = nullptr_t,
            std::enable_if_t<std::conjunction_v<std::is_convertible<T, Type>, std::negation<std::is_same<T, Type>>>, bool> = true
        >
        ICountedPtr(ICountedPtr<T, Deallocator>&& rhs) noexcept
        {
            MoveToThis<ICountedPtr<T, Deallocator>>(std::forward<ICountedPtr<T, Deallocator>>(rhs));
        }
        template<
            typename T, typename Deallocator = nullptr_t,
            std::enable_if_t<std::conjunction_v<std::is_convertible<T, Type>, std::negation<std::is_same<T, Type>>>, bool> = true
        >
        ICountedPtr(const ICountedPtr<T, Deallocator>& rhs)
        {
            if (rhs.IsPtr())
            {
                SetTarget<const ICountedPtr<T, Deallocator>>(rhs.GetTarget());
                if (rhs.GetTarget())
                    rhs.GetTarget()->IncrementCounter();
            }
            else
            {
                SetTarget<const ICountedPtr<T, Deallocator>>(&rhs);
                rhs.IncrementCounter();
            }
        }
        
        // Construct a ICountedPtr, Take over the control of given object pointer.
        ICountedPtr& operator=(Type* &&rhs) noexcept
        {
            HandleSelfRelease();

            SetObject(rhs);

            if (rhs == nullptr)
            {
                // Reset to pointer mode, set nullptr.
                count = UINT64_MAX;
                object = nullptr;
            }
            
            return *this;
        }

        // Dereference the pointer and get the actual object.
        // Only call when IsValid() == True.
        Type& operator*()
        {
            return *(operator->());
        }
        const Type& operator*() const
        {
            return *(operator->());
        }

        // Get the actual pointer of object.
        // Return nullptr if this pointer is invalid.
        Type* operator->()
        {
            if (IsPtr())
            {
                return IsValid() ? GetTarget()->AsTarget() : nullptr;
            }
            else
            {
                return AsTarget();
            }
        }
        const Type* operator->() const
        {
            NON_CONST_MEMBER_CALL_CONST_RET(operator->());
        }
        // Return true if and only if this pointer is valid (has pointed to a valid object)
        bool IsValid() const
        {
            if (IsPtr())
                return GetTarget();
            else
                // Target mode always remains invalid. because a target always holds a 'object' and the count is >= 1.
                return true;
        }
        // bool() wrapper of IsValid().
        operator bool() const noexcept { return IsValid(); }
    private:
        template<typename T1, typename T2> friend class ICountedPtr;
        
        // ========== Pointer Mode Only Functions ========
        // WARNING: Only call the following functions when this is pointer. Otherwise, behavior is undefined.
        NODISCARD FORCEINLINE_DEBUGABLE ICountedPtr* GetTarget() const { return static_cast<ICountedPtr*>(object); }
        // WARNING: This implementation of SetTarget didn't check if type T is same as type Type(self type.)
        // This is to support child ptr can be assigned to parent class ptr.
        // I assume that the passed-in type T has same memory layout as this type (Type).
        // So I did this: Assign ICounterPtr<Child>* to ICounterPtr<Base>*.  THIS relayed on the *same* memory layouts
        // between ICounterPtr<Child> and ICounterPtr<Base>.
        // (In theory, two template class with differ template variable is two non-related classes, they didn't have any relationship)
        // Later we need rethink & rewrite this logic to work 'normally'
        template <typename T> NODISCARD FORCEINLINE_DEBUGABLE void SetTarget(T* other) const
        {
            using RemoveCVType = std::remove_cv_t<T>;
            if constexpr (std::negation_v<std::is_same<RemoveCVType, T>>)
            {
                object = static_cast<void*>(const_cast<RemoveCVType*>(other));
            }
            else
            {
                object = static_cast<void*>(other);
            }
            count = UINT64_MAX;
        }
        
        // ========== Target Mode Only Functions ========
        // WARNING: Only call the following functions when this is target itself. Otherwise, behavior is undefined.

        
        // AsTarget() functions. AsTarget() will return the target pointer itself (which stored in target-mode CountedPtr.)
        // IF this is constant pointer:
        NODISCARD FORCEINLINE_DEBUGABLE const Type* AsTarget() const { return static_cast<const Type*>(object); }
        // IF this is not a constant pointer:
        NODISCARD FORCEINLINE_DEBUGABLE Type* AsTarget() { return static_cast<Type*>(object); }

        
        // Set object ptr, and reset counter to 1.
        NODISCARD FORCEINLINE_DEBUGABLE void SetObject(Type* inObject)
        {
            object = static_cast<void*>(const_cast<std::remove_cv_t<Type>*>(inObject));
            count = 1;
        }
        
        // Increment counter. 
        NODISCARD FORCEINLINE_DEBUGABLE void IncrementCounter() const
        {
            check(!IsPtr());
            ++count;
        }
        // Decrement counter. When decremented counter is zero, it will release target automatically.
        NODISCARD FORCEINLINE_DEBUGABLE void DecrementCounter() const
        {
            check(!IsPtr());
            if(--count == 0)
                Release();
        }
        // Release Target
        NODISCARD FORCEINLINE_DEBUGABLE void Release() const
        {
            auto typedTarget = const_cast<std::remove_cv_t<Type>*>(AsTarget());
            // We assume the target is allocated by C++ new operator
            // TODO: Support delayed release.
            if constexpr (!std::is_same_v<DealloctorFuncType, nullptr_t>)
            {
                if (dealloctorFunc)
                {
                    dealloctorFunc(typedTarget);
                }
            }
            else
            {
                delete typedTarget;
            }
        }
        // ========== Ptr/Target Mode Shared Functions ========
        FORCEINLINE_DEBUGABLE void HandleSelfRelease()
        {
            if (IsPtr())
            {
                if (GetTarget())
                    GetTarget()->DecrementCounter();
            }
            else /* if (!IsPtr()) */
            {
                // I'm target itself.
                DecrementCounter();
            }
        }

        template<typename T>
        FORCEINLINE_DEBUGABLE void MoveToThis(T &&in)
        {
            object = in.object;
            count = in.count;

            in.object = nullptr;
            in.count = UINT64_MAX;
        }
        
        // NOTE To support 'const ICountedPtr<Type>' here, we need modify the following members.
        // REMEMBER 'const ICountedPtr<Type>' is a constant pointer (a pointer that pointed to a constant value), not a pointer constant.
        mutable size_t count{UINT64_MAX}; // count == UINT64_MAX means this is target itself.
        mutable void* object{nullptr};

        DealloctorFuncType dealloctorFunc{nullptr};
    };

    template <typename T, typename... Args>
    ICountedPtr<T> MakeShared(Args&&... args)
    {
        return ICountedPtr<T>(new T(std::forward<Args>(args)...));
    }
}
