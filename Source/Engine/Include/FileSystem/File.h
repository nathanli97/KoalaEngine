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
#include "Core/StringHash.h"

namespace Koala::FileIO
{
    enum class EOpenFileMode
    {
        OpenAsBinary = 1 << 0,
        OpenAsText   = 1 << 1,
        OpenAtAppend = 1 << 2,
    };

    typedef uint32_t EOpenFileModes;
    
    struct FileHandleDataBase
    {
        StringHash    fileName;
        size_t        fileSize;
        EOpenFileMode openMode;

        FileHandleDataBase() = default;
    };

    struct FileReadHandleData: public FileHandleDataBase
    {
        std::ifstream fileStream;
        FORCEINLINE bool IsVaild()
        {
            return fileStream.is_open();
        }
        ~FileReadHandleData()
        {
            fileStream.close();
        }
    };

    struct FileWriteHandleData: public FileHandleDataBase
    {
        std::ofstream fileStream;
        FORCEINLINE bool IsVaild()
        {
            return fileStream.is_open();
        }
        ~FileWriteHandleData()
        {
            fileStream.close();
        }
    };

    typedef std::shared_ptr<FileReadHandleData> FileReadHandle;
    typedef std::shared_ptr<FileWriteHandleData> FileWriteHandle;
    
    class FileManager: public ISingleton
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(FileManager)
        FileReadHandle OpenFileForRead(StringHash path, EOpenFileModes openMode);
        FileWriteHandle OpenFileForWrite(StringHash path, EOpenFileModes openMode);
        
        void CloseFile(FileReadHandle &handle);
        void CloseFile(FileWriteHandle &handle);
    private:
        std::unordered_map<StringHash, FileReadHandle>  openedFilesForRead;
        std::unordered_map<StringHash, FileWriteHandle> openedFilesForWrite;
        std::mutex                                      mutex;
    };
}
