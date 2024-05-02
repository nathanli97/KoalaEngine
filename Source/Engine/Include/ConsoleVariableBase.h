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
#include <shared_mutex>
#include <string>

#include "ThreadNames.h"
#include "Core/Check.h"

namespace Koala
{
    enum EConsoleVariableFlag
    {
        CVF_Default             = 0,
        CVF_ReadOnly            = 1 << 0,
        CVF_TS_MainThread       = 1 << 1,  // This CVar should be considered as thread-safe on MainThread.
        CVF_TS_RenderThread     = 1 << 2,  // This CVar should be considered as thread-safe on RenderThread.
        CVF_TS_RHIThread        = 1 << 3,  // This CVar should be considered as thread-safe on RHIThread.
        CVF_NTS_AnyThread       = 1 << 4,  // This CVar should be considered as non thread-safe on any thread. i.e. Will automatically lock before read&write on anythread.
    };
    typedef uint32_t EConsoleVariableFlags;
    class CVarManager;
    class IConsoleVariable
    {
    public:
        /**
         * Get CVar value. Only call this function after engine is early initialized.
         * @tparam Type What type of data you stored in this CVar?
         * @tparam bDontCheckThreadSafe If true, this function will not to check thread-safe for speed.
         * @return The data stored in this CVar.
         */
        template<typename Type, bool bDontCheckThreadSafe = false>
        const Type& GetAs() const
        {
            if (flags & (uint32_t)EConsoleVariableFlag::CVF_NTS_AnyThread)
            {
                return As_Safe<Type>();
            }

            if constexpr (bDontCheckThreadSafe)
            {
                return As_Unsafe<Type>();
            }
            
            {
                EThreadName name = GetCurrentThreadId();

                //  If this CVar has been tagged as thread-safe in particular thread, and we are in that thread now
                if (
                    (flags & (uint32_t)EConsoleVariableFlag::CVF_TS_MainThread && name == EThreadName::MainThread) ||
                    (flags & (uint32_t)EConsoleVariableFlag::CVF_TS_RenderThread && name == EThreadName::RenderThread) ||
                    (flags & (uint32_t)EConsoleVariableFlag::CVF_TS_RHIThread && name == EThreadName::RHIThread))
                    return As_Unsafe<Type>();
            }

            return As_Safe<Type>();
        }
        template<typename Type>
        const Type& GetAs_MainThread() const
        {
            if (flags & (uint32_t)EConsoleVariableFlag::CVF_TS_MainThread)
                return GetAs<Type, true>();
            else
                return As_Safe<Type>();
        }
        template<typename Type>
        const Type& GetAs_RenderThread() const
        {
            if (flags & (uint32_t)EConsoleVariableFlag::CVF_TS_RenderThread)
                return GetAs<Type, true>();
            else
                return As_Safe<Type>();
        }
        template<typename Type>
        const Type& GetAs_RHIThread() const
        {
            if (flags & (uint32_t)EConsoleVariableFlag::CVF_TS_RHIThread)
                return GetAs<Type, true>();
            else
                return As_Safe<Type>();
        }
        
        template<typename Type>
        void Set(const Type &inValue)
        {
            if (flags & (uint32_t)EConsoleVariableFlag::CVF_ReadOnly)
            {
                check(false, "You can not set this CVar, because this is read-only CVar.");
                return;
            }
            Set_Safe(inValue);
        }
    protected:
        friend class CVarManager;
        
        IConsoleVariable(EConsoleVariableFlags inFlags): flags(inFlags) {}
        template<typename T>
        const T& As_Unsafe() {return *static_cast<const T*>(storage);}
        
        template<typename T>
        T& As_Unsafe() {return *static_cast<T*>(storage);}

        template<typename T>
        const T& As_Safe() const
        {
            std::shared_lock lock(mutex);
            return As_Unsafe<T>();
        }

        template<typename T>
        void Set_Safe(const T& inValue)
        {
            std::unique_lock lock(mutex);
            As_Unsafe<T>() = inValue;
        }
        void* storage{nullptr};
        std::string name, helper;
        EConsoleVariableFlags flags;
    private:
        mutable std::shared_mutex mutex;
        static EThreadName GetCurrentThreadId();
    };
}
