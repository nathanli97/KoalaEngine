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

namespace Koala::FileIO
{
    FileHandle FileManager::OpenFile(StringHash path, EOpenFileMode openMode)
    {
        std::unique_lock lock(mutex);
        if (!openedFiles.contains(path))
        {
            auto handle = std::make_shared<FileHandleData>();
            std::ios::openmode mode{};
            switch (openMode)
            {
            case EOpenFileMode::Read:
                mode = std::ios::in;
                break;
            case EOpenFileMode::Write:
                mode = std::ios::out;
                break;
            case EOpenFileMode::ReadAndWrite:
                mode = std::ios::in | std::ios::out;
                break;
            }
            handle->fileStream.open(path.GetString(), mode | std::ios::binary);
            openedFiles.emplace(path, handle);
            return handle;
        }
        else
            return openedFiles[path];
    }

    void FileManager::CloseFile(StringHash path)
    {
        std::unique_lock lock(mutex);
        if (openedFiles.contains(path))
        {
            openedFiles.erase(path);
        }
    }

    void FileManager::CloseFile(FileHandle &handle)
    {
        CloseFile(handle->fileName);
    }
}
