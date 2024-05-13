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
#include <unordered_map>

#include "Definations.h"
#include "SingletonInterface.h"

namespace Koala
{
    constexpr uint64_t FNVOffsetBasis = 14695981039346656037ULL;
    constexpr uint64_t FNVPrime = 1099511628211ULL;
    FORCEINLINE uint64_t HashString(std::string str) {
        uint64_t hash = FNVOffsetBasis;
        for (int8_t c: str)
        {
            hash = hash ^ static_cast<uint64_t>(c);
            hash *= FNVPrime;
        }
        return hash;
    }
    
    class StringHashPool: ISingleton
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(StringHashPool)
        FORCEINLINE size_t RegisterString(const std::string &inStr)
        {
            std::unique_lock lock(mutex);
            auto hash = HashString(inStr);
            if (!stringPool.contains(hash))
                stringPool.emplace(hash, inStr);
            return hash;
        }
        FORCEINLINE bool GetString(size_t inHash, std::string &outStr)
        {
            std::shared_lock lock(mutex);
            if (stringPool.contains(inHash))
            {
                outStr = stringPool.at(inHash);
                return true;
            }
            return false;
        }
    private:
        std::unordered_map<size_t, std::string> stringPool;
        std::shared_mutex mutex;
    };
    class StringHash final
    {
    public:
        StringHash() = default;
        StringHash(const StringHash &rhs) = default;
        StringHash(StringHash &&rhs) = default;
        StringHash& operator=(const StringHash&) = default;
        StringHash& operator=(StringHash&&) = default;
        
        inline bool operator==(const StringHash &rhs) const
        {
            return hash == rhs.hash;
        }
        inline bool operator!=(const StringHash& rhs) const {return !operator==(rhs);}
        size_t GetHash() const {return hash;}
        StringHash(size_t inHash): hash(inHash) {}
        StringHash(const std::string& inStr)
        {
            hash = StringHashPool::Get().RegisterString(inStr);
        }
        FORCEINLINE std::string GetString()
        {
            std::string str;
            StringHashPool::Get().GetString(hash, str);
            return str;
        }
    private:
        size_t hash{0};
    };
    
}

template<>
    struct std::hash<Koala::StringHash>
{
    std::size_t operator()(const Koala::StringHash& inHash) const noexcept
    {
        return inHash.GetHash();
    }
};