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
#include <unordered_map>

#include "Core/ModuleInterface.h"
#include "Core/HashedString.h"
#include "Core/ThreadInterface.h"


namespace Koala::FileIO
{
    enum EFileOpenMode
    {
        OpenFileAsBinary = 1 << 0,
        OpenFileAsText   = 1 << 1,
        OpenFileAtAppend = 1 << 2,
        OpenFileForRead   = 1 << 3,
        OpenFileForWrite  = 1 << 4,
    };
    typedef uint32_t EOpenFileModes;

    enum class EFilePriority
    {
        Highest = 5,
        High    = 4,
        Normal  = 3,
        Low     = 2,
        Lowest  = 1
    };

    struct FileHandleData
    {
        HashedString     fileName;
        size_t         fileSize;
        EOpenFileModes openMode;
        EFilePriority  priority{EFilePriority::Normal};
        std::fstream   fileStream;

        IThread        *currWorkingIOThread{nullptr};


        FileHandleData() = default;

        FORCEINLINE bool IsValid() const
        {
            return fileName.GetHash() != 0 && fileStream.is_open() && fileStream.good();
        }

        FORCEINLINE bool IsEOF() const
        {
            return fileStream.eof();
        }

        FORCEINLINE bool IsOpened() const
        {
            return fileStream.is_open();
        }

        FORCEINLINE bool IsOpenedForReadOnly() const
        {
            return openMode & EFileOpenMode::OpenFileForRead &&
                !(openMode & EFileOpenMode::OpenFileForWrite);
        }

        FORCEINLINE bool CanRead() const
        {
            return IsValid() && openMode & (uint32_t)EFileOpenMode::OpenFileForRead;
        }

        FORCEINLINE bool CanWrite() const
        {
            return IsValid() && openMode & (uint32_t)EFileOpenMode::OpenFileForWrite;
        }
    };

    typedef std::shared_ptr<FileHandleData> FileHandle;

    class FileManager: public ISingleton
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(FileManager)
        FileHandle OpenFileForRead(HashedString path, EOpenFileModes openMode = EFileOpenMode::OpenFileAsBinary);
        FileHandle OpenFileForWrite(HashedString path, EOpenFileModes openMode = EFileOpenMode::OpenFileAsBinary);
        
        void CloseFile(FileHandle &handle);
    private:
        void CalcFileSize(FileHandle & inHandle);

        std::unordered_map<HashedString, FileHandle>  openedFilesForRead;
        std::unordered_map<HashedString, FileHandle> openedFilesForWrite;
        std::mutex                                      mutex;
    };
}
