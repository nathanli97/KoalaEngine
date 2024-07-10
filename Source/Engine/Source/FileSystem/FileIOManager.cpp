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
#include "FileSystem/FileIOManager.h"

#include "Core/ThreadManager.h"
#include "FileSystem/FileIOThread.h"

namespace Koala::FileIO
{
    bool FileIOManager::Initialize_MainThread()
    {
        auto numCPUCores = std::thread::hardware_concurrency();

        numReadThreads = std::min(numCPUCores, (uint32_t)IOThreadNumDiskGradeMapping.at(diskSpeedGrade).first);
        numWriteThreads = std::min(numCPUCores, (uint32_t)IOThreadNumDiskGradeMapping.at(diskSpeedGrade).second);
        
        for (auto i = 0; i < numReadThreads; i++)
        {
            auto handle = new FileIOThread();
            readThreadHandles.push_back(handle);
            ThreadManager::Get().CreateThreadManaged(handle);
        }

        for (auto i = 0; i < numWriteThreads; i++)
        {
            auto handle = new FileIOThread();
            readThreadHandles.push_back(handle);
            ThreadManager::Get().CreateThreadManaged(handle);
        }
        return true;
    }

    bool FileIOManager::Shutdown_MainThread()
    {
        for (auto handle: readThreadHandles)
        {
            auto t = static_cast<FileIOThread*> (handle);
            t->ShutdownIOThread();
        }

        for (auto handle: writeThreadHandles)
        {
            auto t = static_cast<FileIOThread*> (handle);
            t->ShutdownIOThread();
        }
    }

    void FileIOManager::Tick_MainThread(float /*deltaTime*/) {}
    
}
