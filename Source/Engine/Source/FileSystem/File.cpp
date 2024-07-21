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

#include "FileSystem/File.h"

#include "Core/Check.h"

namespace Koala::FileIO
{
    static Logger logger("FileIO");
    FileHandle FileManager::OpenFileForRead(StringHash path, EOpenFileModes openMode)
    {
        check(openMode & (uint32_t)EOpenFileMode::OpenAsRead);

        std::scoped_lock lock(mutex);
        if (openedFilesForWrite.contains(path))
        {
            logger.error("Failed to open file {} for read because this file is already opened for write", path.GetString());
            return nullptr;
        }
        if (openedFilesForRead.contains(path))
            return openedFilesForRead[path];
        else
        {
            std::fstream stream;
            std::ios::openmode mode{};
            if (openMode & (uint32_t)EOpenFileMode::OpenAsBinary)
            {
                mode = std::ios::binary;
            }

            if (openMode & (uint32_t)EOpenFileMode::OpenAtAppend)
            {
                // Ignore.
            }

            mode |= std::ios::in;

            stream.open(path.GetString(), mode);

            if (!stream.is_open())
            {
                logger.error("Failed to open file {} for read because this file cannot be opened for read (file not exist or I/O error)", path.GetString());
                return nullptr;
            }
            
            auto handle = std::make_shared<FileHandleData>();
            handle->fileName = path;
            handle->fileSize = 0;
            handle->fileStream = std::move(stream);

            
            openedFilesForRead.emplace(path, handle) ;

            return handle;
        }
    }

    FileHandle FileManager::OpenFileForWrite(StringHash path, EOpenFileModes openMode)
    {
        check(openMode & (uint32_t)EOpenFileMode::OpenAsWrite);

        std::scoped_lock lock(mutex);
        if (openedFilesForRead.contains(path))
        {
            logger.error("Failed to open file {} for write because this file is already opened for reac", path.GetString());
            return nullptr;
        }
        if (openedFilesForWrite.contains(path))
            return openedFilesForWrite[path];
        else
        {
            std::fstream stream;
            std::ios::openmode mode{};
            if (openMode & (uint32_t)EOpenFileMode::OpenAsBinary)
            {
                mode = std::ios::binary;
            }

            if (openMode & (uint32_t)EOpenFileMode::OpenAtAppend)
            {
                mode |= std::ios::app;
            }

            mode |= std::ios::out;

            stream.open(path.GetString(), mode);

            if (!stream.is_open())
            {
                logger.error("Failed to open file {} for write because this file cannot be opened for write (file not exist or I/O error)", path.GetString());
                return nullptr;
            }
            
            auto handle = std::make_shared<FileHandleData>();
            handle->fileName = path;
            handle->fileSize = 0;
            handle->fileStream = std::move(stream);

            
            openedFilesForWrite.emplace(path, handle) ;

            return handle;
        }
    }

    void FileManager::CloseFile(FileHandle &handle)
    {
        if (handle->fileStream.is_open())
            handle->fileStream.close();
        {
            std::scoped_lock lock(mutex);
            if (handle->IsOpenedForReadOnly())
            {
                if (openedFilesForRead.contains(handle->fileName))
                {
                    openedFilesForRead.erase(handle->fileName);
                }
            }
            else
            {
                if (openedFilesForWrite.contains(handle->fileName))
                {
                    openedFilesForWrite.erase(handle->fileName);
                }
            }
        }
    }
}
