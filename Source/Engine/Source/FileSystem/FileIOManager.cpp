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

        numReadThreads = std::min(numCPUCores, numReadThreads);
        numWriteThreads = std::min(numCPUCores, numWriteThreads);
        
        for (uint32_t i = 0; i < numReadThreads; i++)
        {
            auto handle = new FileIOThread();
            readThreadHandles.push_back(handle);
            ThreadManager::Get().CreateThreadManaged(handle);
        }

        for (uint32_t i = 0; i < numWriteThreads; i++)
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
            auto t = dynamic_cast<FileIOThread*> (handle);
            t->ShutdownIOThread();
        }

        for (auto handle: writeThreadHandles)
        {
            auto t = dynamic_cast<FileIOThread*> (handle);
            t->ShutdownIOThread();
        }

        return true;
    }

    void FileIOManager::Tick_MainThread(float /*deltaTime*/)
    {
        std::vector<FileIOThread*> idleReadThreads, idleWriteThreads;
        idleReadThreads.reserve(numReadThreads);
        idleWriteThreads.reserve(numWriteThreads);
        
        for (auto handle: readThreadHandles)
        {
            auto t = dynamic_cast<FileIOThread*> (handle);
            if (t->IsIdle())
                idleReadThreads.push_back(t);
        }

        for (auto handle: writeThreadHandles)
        {
            auto t = dynamic_cast<FileIOThread*> (handle);
            if (t->IsIdle())
                idleWriteThreads.push_back(t);
        }

        if (idleReadThreads.size() > 0)
        {
            for (auto &thread: idleReadThreads)
            {
                if (remainingReadTasks.empty())
                    break;
                
                auto task = remainingReadTasks.front();
                remainingReadTasks.pop();

                if (!task.handle->IsVaild())
                    continue;
                
                auto func = [task]()
                {
                    if (!task.handle->IsVaild())
                        return;

                    constexpr size_t BlockSize = 4096;
                    
                };
            }
        }

        if (idleWriteThreads.size() > 0)
        {
            
        }
        
        
    }

    void FileIOManager::TickFileReadIOThread(size_t threadIdx)
    {
        FileIOThread* thread = dynamic_cast<FileIOThread*> (readThreadHandles.at(threadIdx));
        
    }
}
