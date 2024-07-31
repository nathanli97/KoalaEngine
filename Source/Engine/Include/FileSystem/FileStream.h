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

#include <fstream>
#include <utility>

#include "Definations.h"
#include "File.h"
#include "FileIOManager.h"
#include "FileIOTask.h"
#include "Core/ByteStream.h"
#include "Core/Check.h"
#include "Memory/MemoryPool.h"

namespace Koala::FileIO
{
    class FileByteStream: public IByteStream
    {
    public:
        size_t Write(const void *src, size_t size) override;
        size_t Read(void *dst, size_t size) override;
        size_t Tell() const override;
        void SeekFromBegin(size_t offset) const override;
        void SeekFromCurrent(size_t offset) const override;
        void SeekFromEnd(size_t offset) const override;
        bool IsOpened() const override;
        bool IsEOF() const override;
        void Close() const override;
        bool IsReadOnly() const override;
    private:
    };
}
