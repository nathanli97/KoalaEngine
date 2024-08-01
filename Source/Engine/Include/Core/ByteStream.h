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

#include "Check.h"
#include "Definations.h"

namespace Koala
{
    class IByteStream
    {
    public:
        virtual ~IByteStream() {}
        
        virtual size_t Write(const void *src, size_t size) = 0;
        virtual size_t Read(void *dst, size_t size) = 0;
        virtual size_t Tell() const = 0;
        virtual void SeekFromBegin(size_t offset) = 0;
        virtual void SeekFromCurrent(size_t offset) = 0;
        virtual void SeekFromEnd(size_t offset) = 0;
        virtual bool IsOpened() const = 0;
        virtual bool IsEOF() const = 0;
        virtual void Close() = 0;
        virtual bool IsReadOnly() const = 0;

        virtual bool IsGood() const
        {
            if (IsReadOnly())
                return IsOpened() && !IsEOF();
            else
                return IsOpened();
        }

        NODISCARD FORCEINLINE bool CanRead() const
        {
            return IsGood() && !IsEOF();
        }

        NODISCARD FORCEINLINE bool CanWrite() const
        {
            return !IsReadOnly() && IsGood();
        }

        template <typename Type>
        IByteStream& operator<<(const Type &value)
        {
            check(CanWrite());
            Write(&value, sizeof(Type));
            return *this;
        }

        template <typename Type>
        IByteStream& operator>>(Type &dst)
        {
            check(CanRead());
            Read(&dst, sizeof(Type));
            return *this;
        }
    };
}