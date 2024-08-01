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
    // Unbuffered FileByteStream implementation
    class FileByteStream: public IByteStream
    {
    public:
        FileByteStream(const FileHandle &inHandle): fileHandle(inHandle) {}
        size_t Write(const void *src, size_t size) override;
        size_t Read(void *dst, size_t size) override;
        size_t Tell() const override
        {
            check(IsOpened());
            return currOffset;
        }
        void SeekFromBegin(size_t offset) override
        {
            check(IsOpened());
            currOffset = offset;
            check(!fileHandle->fileSize || currOffset < fileHandle->fileSize);
        }
        void SeekFromCurrent(size_t offset) override
        {
            check(IsOpened());
            currOffset += offset;
            check(!fileHandle->fileSize || currOffset < fileHandle->fileSize);
        }
        void SeekFromEnd(size_t offset) override
        {
            check(IsOpened());
            if (fileHandle->CanRead() && fileHandle->fileSize != 0)
            {
                check(offset < fileHandle->fileSize);
                currOffset = fileHandle->fileSize - offset;
            }
            else
            {
                currOffset = UINT64_MAX;
            }
        }
        NODISCARD FORCEINLINE bool IsOpened() const override
        {
            return fileHandle && fileHandle->IsOpened();
        }
        NODISCARD FORCEINLINE bool IsEOF() const override
        {
            return fileHandle->IsEOF();
        }
        NODISCARD FORCEINLINE void Close() override
        {
            fileHandle = nullptr;
        }
        NODISCARD FORCEINLINE bool IsReadOnly() const override
        {
            return fileHandle->IsOpenedForReadOnly();
        }
    private:
        FileHandle fileHandle;
        size_t currOffset{0};
    };
}
