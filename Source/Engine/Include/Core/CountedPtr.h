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
#define KOALA_COUNTER_PTR_STORAGE_INVALID_COUNTER UINT64_MAX
namespace Koala
{
    struct CounterPtrStorage
    {
        mutable size_t counter{0};
        mutable void *object{nullptr};

        CounterPtrStorage() = default;
        NODISCARD FORCEINLINE CounterPtrStorage(CounterPtrStorage&& other) noexcept
        {
            counter = other.counter;
            object = other.object;
            other.ResetToNullReference();
        }
        template <typename T> T GetAs() const
        {
            return static_cast<T>(object);
        }
        FORCEINLINE void ResetToNullReference() const
        {
            counter = KOALA_COUNTER_PTR_STORAGE_INVALID_COUNTER;
            object = nullptr;
        }
        NODISCARD FORCEINLINE bool IsReference() const {return counter == KOALA_COUNTER_PTR_STORAGE_INVALID_COUNTER;}
        NODISCARD FORCEINLINE bool IsTarget() const {return !IsReference();}
        CounterPtrStorage* GetAsReference() const {return static_cast<CounterPtrStorage*>(object);}
        CounterPtrStorage* GetReferenceToTarget() const
        {
            if (IsTarget())
                return const_cast<CounterPtrStorage*>(this);
            else
            {
                return GetAsReference() ? GetAsReference() : nullptr;
            }
        }
        size_t GetTargetCounter() const
        {
            auto target = GetReferenceToTarget();
            return target ? target->counter : 0;
        }
        FORCEINLINE void SetTargetReference(CounterPtrStorage* referenceToTarget) const
        {
            counter = KOALA_COUNTER_PTR_STORAGE_INVALID_COUNTER;
            object = static_cast<void*>(referenceToTarget);
        }
        FORCEINLINE void IncrementCounterAsThisIsTarget() const
        {
            check(IsTarget());
            ++counter;
        }
        FORCEINLINE void DecrementCounterAsThisIsTarget() const
        {
            check(IsTarget());
            --counter;
        }
        FORCEINLINE void FreeObjectAsThisIsTarget() const
        {
            check(IsTarget());
            free(object);
        }
        template<typename ObjectType>
        FORCEINLINE void DeleteObjectAsThisIsTarget() const
        {
            check(IsTarget());
            delete static_cast<ObjectType>(object);
        }
    };
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
        NODISCARD FORCEINLINE_DEBUGABLE size_t GetCounter() const
        {
            return storage.GetTargetCounter();
        }
        ~ICountedPtr()
        {
            HandleSelfRelease();
        }
        ICountedPtr(nullptr_t): storage(){}
        ICountedPtr& operator=(nullptr_t)
        {
            HandleSelfRelease();
            return *this;
        }
        ICountedPtr(Type* inObject, DealloctorFuncType inDealloctorFunc = nullptr):
            storage(),
            dealloctorFunc(inDealloctorFunc)
        {
            if (inObject == nullptr)
            {
                // Reset to pointer mode
                storage.ResetToNullReference();
            }
        }
        ICountedPtr() {}
        ICountedPtr(const ICountedPtr& rhs)
        {
            if (rhs.storage.IsReference())
            {
                storage.SetTargetReference(rhs.storage.GetReferenceToTarget());
                if (rhs.storage.GetReferenceToTarget())
                    rhs.storage.GetReferenceToTarget()->IncrementCounterAsThisIsTarget();
            }
            else /* rhs.storage.IsTarget() */
            {
                storage.SetTargetReference(&rhs.storage);
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
            MoveToThis<ICountedPtr<T, Deallocator>>(std::move<ICountedPtr<T, Deallocator>&&>(rhs));
        }
        template<
            typename T, typename Deallocator = nullptr_t,
            std::enable_if_t<std::conjunction_v<std::is_convertible<T, Type>, std::negation<std::is_same<T, Type>>>, bool> = true
        >
        ICountedPtr(const ICountedPtr<T, Deallocator>& rhs)
        {
            if (rhs.storage.IsReference())
            {
                storage.SetTargetReference(rhs.storage.GetReferenceToTarget());
                if (rhs.storage.GetReferenceToTarget())
                    rhs.storage.GetReferenceToTarget()->IncrementCounterAsThisIsTarget();
            }
            else /* rhs.storage.IsTarget() */
            {
                storage.SetTargetReference(&rhs.storage);
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
                storage.ResetToNullReference();
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
           return GetTargetObject();
        }
        const Type* operator->() const
        {
            NON_CONST_MEMBER_CALL_CONST_RET(operator->());
        }
        // Return true if and only if this pointer is valid (has pointed to a valid object)
        bool IsValid() const
        {
            return storage.GetReferenceToTarget();
        }
        // bool() wrapper of IsValid().
        operator bool() const noexcept { return IsValid(); }
    private:
        template<typename T1, typename T2> friend class ICountedPtr;
        
        NODISCARD FORCEINLINE_DEBUGABLE const Type* GetTargetObject() const
        {
            if (storage.IsTarget())
                return storage.GetAs<const Type*>();
            else
            {
                if (storage.GetReferenceToTarget())
                    return storage.GetReferenceToTarget()->GetAs<const Type*>();
                return nullptr;
            }
        }
        // IF this is not a constant pointer:
        NODISCARD FORCEINLINE_DEBUGABLE Type* GetTargetObject()
        {
            NON_CONST_MEMBER_CALL_CONST_RET(GetTargetObject());
        }

        
        // Set object ptr, and reset counter to 1.
        FORCEINLINE_DEBUGABLE void SetObject(Type* inObject)
        {
            storage.object = static_cast<void*>(const_cast<std::remove_cv_t<Type>*>(inObject));
            storage.counter = 1;
        }

        // Release Target in storage
        FORCEINLINE_DEBUGABLE void Release() const
        {
            auto typedTarget = const_cast<std::remove_cv_t<Type>*>(GetTargetObject());
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
        FORCEINLINE_DEBUGABLE void HandleSelfRelease()
        {
            if (storage.IsReference())
            {
                if(storage.GetReferenceToTarget())
                    storage.GetReferenceToTarget()->DecrementCounterAsThisIsTarget();
            }
            else
            {
                // storage is target.
                storage.DecrementCounterAsThisIsTarget();
            }
        }

        template<typename T>
        FORCEINLINE_DEBUGABLE void MoveToThis(T &&in)
        {
            storage = std::move(in.storage);
            
            in.storage.ResetToNullReference();
        }
        
        // NOTE To support 'const ICountedPtr<Type>' here, we need modify the following members.
        // REMEMBER 'const ICountedPtr<Type>' is a constant pointer (a pointer that pointed to a constant value), not a pointer constant.
        mutable CounterPtrStorage storage{};

        DealloctorFuncType dealloctorFunc{nullptr};
    };

    template <typename T, typename... Args>
    ICountedPtr<T> MakeShared(Args&&... args)
    {
        return ICountedPtr<T>(new T(std::forward<Args>(args)...));
    }
}
