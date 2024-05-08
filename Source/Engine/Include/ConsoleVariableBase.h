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

#include "Core/ThreadTypes.h"
#include "Core/Check.h"

namespace Koala
{
    enum EConsoleVariableFlag
    {
        CVF_Default             = 0,
        CVF_ReadOnly            = 1 << 0,
        CVF_TS_MainThread       = 1 << 1,  // This CVar should be considered as thread-safe on MainThread. NOT USED NOW.
        CVF_TS_RenderThread     = 1 << 2,  // This CVar should be considered as thread-safe on RenderThread. NOT USED NOW.
        CVF_TS_RHIThread        = 1 << 3,  // This CVar should be considered as thread-safe on RHIThread. NOT USED NOW.
        CVF_NTS_AnyThread       = 1 << 4,  // This CVar should be considered as non thread-safe on any thread. NOT USED NOW.
    };
    typedef uint32_t EConsoleVariableFlags;
    class CVarManager;
    class IConsoleVariable
    {
    public:
        virtual ~IConsoleVariable() = default;
        template <typename T>
        T Get() const
        {
            using RemoveConstRefType = std::remove_const_t<std::remove_reference_t<T>>;
            if constexpr (std::is_arithmetic_v<RemoveConstRefType>)
            {
                if (IsArithmetic())
                {
                    if constexpr (sizeof(RemoveConstRefType) < 8)
                    {
                        return (T)(GetArithmetic());
                    }
                    else
                    {
                        return GetArithmetic();
                    }
                }
                else
                {
                    return 0;
                }
            }
            else if constexpr (std::is_same_v<RemoveConstRefType, std::string>)
            {
                if (!IsArithmetic())
                    return GetString();
                else
                {
                    return "";
                }
            }
            else
            {
                return 0;
            }
        }
        template <typename T>
        void Set(T inValue)
        {
            if (flags & (uint32_t)EConsoleVariableFlag::CVF_ReadOnly)
            {
                check(false, "This CVAR is read-only!");
                return;
            }
            
            using RemoveConstRefType = std::remove_const_t<std::remove_reference_t<T>>;
            if constexpr (std::is_arithmetic_v<RemoveConstRefType>)
            {
                if (IsArithmetic())
                {
                    SetArithmetic(inValue);
                }
            }
            else if constexpr (std::is_same_v<RemoveConstRefType, std::string>)
            {
                if (!IsArithmetic())
                {
                    SetString(inValue);
                }
            }
        }

        IConsoleVariable() = delete;
        IConsoleVariable(const IConsoleVariable&) = delete;
        IConsoleVariable(IConsoleVariable&&) = delete;
        const IConsoleVariable& operator=(const IConsoleVariable&) = delete;
        IConsoleVariable&& operator=(IConsoleVariable&&) = delete;
    protected:
        friend class CVarManager;
        
        virtual size_t GetArithmetic() const = 0;
        virtual std::string GetString() const = 0;
        virtual void SetArithmetic(size_t i) = 0;
        virtual void SetString(std::string inStr) = 0;
        virtual bool IsArithmetic() const = 0;
        
        IConsoleVariable(EConsoleVariableFlags inFlags, const std::string &inName, const std::string &inHelper)
            : name(inName), helper(inHelper), flags(inFlags) {}
        
        std::string name, helper;
        EConsoleVariableFlags flags;
    };

    class IConsoleVariableString: public IConsoleVariable
    {
    public:
        IConsoleVariableString(EConsoleVariableFlags inFlags, const std::string &inName, const std::string &inHelper)
            : IConsoleVariable(inFlags, inName, inHelper) {}
    protected:
        FORCEINLINE size_t GetArithmetic() const override { return 0; }
        FORCEINLINE std::string GetString() const override
        {
            std::shared_lock lock(mutex);
            return value;
        }
        FORCEINLINE bool IsArithmetic() const override {return false;}
        FORCEINLINE void SetArithmetic(size_t i) override {}
        FORCEINLINE void SetString(std::string inStr) override
        {
            std::unique_lock lock(mutex);
            value = inStr;
        }
    private:
        std::string value;
        mutable std::shared_mutex mutex;
    };
    class IConsoleVariableArithmetic: public IConsoleVariable
    {
    public:
        IConsoleVariableArithmetic(EConsoleVariableFlags inFlags, const std::string &inName, const std::string &inHelper)
            : IConsoleVariable(inFlags, inName, inHelper) {}
    protected:
        FORCEINLINE size_t GetArithmetic() const override
        {
            return value.load(std::memory_order::acquire);
        }
        FORCEINLINE std::string GetString() const override
        {
            return "";
        }
        FORCEINLINE void SetArithmetic(size_t i) override
        {
            value.store(i, std::memory_order::consume);
        }
        FORCEINLINE void SetString(std::string inStr) override {}
        FORCEINLINE bool IsArithmetic() const override
        {
            return true;
        }

    private:
        std::atomic<size_t> value;
    };
}
