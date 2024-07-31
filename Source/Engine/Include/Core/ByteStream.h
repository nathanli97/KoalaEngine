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
#include <deque>

#include "Definations.h"

namespace Koala
{
    class ByteStream
    {
    public:
        size_t Write(const char *src, size_t size);
        size_t Read(void *dst, size_t size);

        NODISCARD FORCEINLINE bool CanRead(const size_t requiredSize = 1) const
        {
            return GetSize() > requiredSize;
        }
        NODISCARD FORCEINLINE size_t GetSize() const
        {
            return deque.size();
        }

        template <typename Type>
        ByteStream& operator<<(const Type &value)
        {
            Write(&value, sizeof(Type));
            return *this;
        }

        template <typename Type>
        ByteStream& operator>>(Type &dst)
        {
            Read(&dst, sizeof(Type));
            return *this;
        }
    private:
        std::deque<char> deque;
    };
}