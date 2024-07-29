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
#include "Core/Check.h"
#include "Memory/MemoryPool.h"

namespace Koala::FileIO
{
    constexpr size_t FileStreamPageSize = 4096;
    class FileStream
    {
    public:
        FileStream() = default;
        FileStream(FileHandle &inHandle): handle(inHandle){}
        inline FileStream & operator=(const FileHandle & rhs)
        {
            handle = rhs;
            Initialize();
            return *this;
        }

        virtual inline void Initialize(){}

        NODISCARD FORCEINLINE bool IsValid() const
        {
            return handle != nullptr && handle->IsValid();
        }

        FORCEINLINE void Seek(size_t num)
        {
            ensure(IsValid());

            offset = num;
        }

        FORCEINLINE size_t Tell() const
        {
            ensure(IsValid());
            return offset;
        }

        FORCEINLINE size_t GetFileSize() const
        {
            ensure(IsValid());
            return handle->fileSize;
        }
        
        template <typename T>
        FileStream & operator<<(T &v)
        {
            return *this;
        }

        // This function is only intend to serialize small memory fields.
        virtual void Serialize(void* buf, size_t size) = 0;
    
    protected:
        FileHandle              handle{nullptr};
        size_t                  offset{0};
    };

    class ReadFileStream: public FileStream
    {
    public:
        using FileStream::operator=;
        ReadFileStream()
        {
            FileStream::Initialize();
        }
        ReadFileStream(FileHandle &inHandle): FileStream(inHandle)
        {
            FileStream::Initialize();
        }
        
        ReadFileStream(HashedString filePath, bool bOpenAsText = false)
        {
            handle = FileManager::Get().OpenFileForRead(filePath, bOpenAsText ? EFileOpenMode::OpenFileAsText : EFileOpenMode::OpenFileAsBinary);
        }

        inline void Initialize() override
        {}
        
        FORCEINLINE ReadFileStream& ReadAsync(void* inBuffer, size_t inSize, FileIOCallback callback = nullptr)
        {
            ensure(IsValid());
            FileIOManager::Get().RequestReadFileAsync(handle, offset, inSize, inBuffer, std::move(callback));
            return *this;
        }


    };

    class WriteFileStream: public FileStream
    {
    public:
        using FileStream::FileStream;
        using FileStream::operator=;
        WriteFileStream(HashedString filePath, bool bOpenAsText = false)
        {
            handle = FileManager::Get().OpenFileForWrite(filePath, bOpenAsText ? EFileOpenMode::OpenFileAsText : EFileOpenMode::OpenFileAsBinary);
        }
        FORCEINLINE WriteFileStream& WriteAsync(const void* inBuffer, size_t inSize, FileIOCallback callback = nullptr)
        {
            ensure(IsValid());
            FileIOManager::Get().RequestWriteFileAsync(handle, offset, inSize, inBuffer, callback);
            return *this;
        }
    };
}
