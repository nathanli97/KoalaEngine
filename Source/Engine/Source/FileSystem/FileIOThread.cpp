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

#include "FileSystem/FileIOThread.h"

namespace Koala::FileIO
{
    constexpr int64_t BlockSize = 4096;
    constexpr int64_t MaxContinuousIOWorkBlocks = 64;

    int64_t FilePriorityToBlockNum(EFilePriority inPriority)
    {
        switch (inPriority)
        {
        case EFilePriority::Highest:
            return 64;
        case EFilePriority::High:
            return 32;
        case EFilePriority::Normal:
            return 16;
        case EFilePriority::Low:
            return 8;
        case EFilePriority::Lowest:
            return 4;
        default: return 1;
        }
    }
    void FileIOThread::Run()
    {
        while (!atomicShouldShutdown.load())
        {
            while (atomicAwakeSignal.load() == false)
                atomicAwakeSignal.wait(false);
            DoWork();
        }
    }

    void FileIOThread::ShutdownIOThread()
    {
        atomicShouldShutdown.store(true);
        atomicShouldShutdown.notify_all();
        atomicAwakeSignal.store(true);
        atomicAwakeSignal.notify_all();
    }


    void FileIOThread::DoWork()
    {
        FileIOTask task;
        
        {
            std::lock_guard lock(mutexTQ);
            if (taskQueue.empty())
            {
                // Put the thread to sleep until we has new task.
                atomicAwakeSignal.store(false);
                return;
            }

            task = std::move(taskQueue.front());
            taskQueue.pop();

            atomicTQLength.fetch_sub(1);
        }

        if (!task.bOK || task.bCanceled)
        {
            std::lock_guard lock(mutexFinishedTaskList);
            finishedTasks.push_back(task);
            
            return;
        }

        if (task.bCancelRequested)
        {
            task.bOK = false;
            task.bCanceled = true;
            task.bFinished = true;
            
            {
                std::lock_guard lock(mutexFinishedTaskList);
                finishedTasks.push_back(task);
            }
            
            return;
        }
        
        
        if (task.remainingSize != 0 && task.bufferStart)
        {
            
            FileHandle handle = task.handle;
            
            int64_t blocks = std::min(task.remainingSize / BlockSize, FilePriorityToBlockNum(task.handle->priority));
            blocks = std::min(blocks, MaxContinuousIOWorkBlocks);
            
            if (blocks == 0)
                blocks = 1;

            int64_t remainingSize = task.remainingSize;
            int64_t size = 0;

            if (bIsReadThread)
            {
                char * buffer = static_cast<char*>(task.bufferStart) + task.offset;
                for (auto i = 0; i < blocks; ++i)
                {
                    int64_t blockSize = std::min(BlockSize, remainingSize);

                    handle->fileStream.read(buffer + size, blockSize);
                    int64_t readSize = handle->fileStream.gcount();
                    size += readSize;
                    remainingSize -= readSize;
                }
            }
            else
            {
                const char * buffer = static_cast<const char*>(task.bufferStart) + task.offset;
                for (auto i = 0; i < blocks; ++i)
                {
                    int64_t blockSize = std::min(BlockSize, remainingSize);

                    handle->fileStream.write(buffer + size, blockSize);
                    int64_t writeSize = handle->fileStream.gcount();
                    size += writeSize;
                    remainingSize -= writeSize;
                }
            }
            
            task.offset += size;
            task.performedSize += size;
            task.remainingSize = remainingSize;

            if (remainingSize == 0)
            {
                task.bOK = true;
                task.bFinished = true;
                task.bCompleted = true;
            }
            
            if (handle->IsEOF() || !handle->IsValid())
            {
                task.bOK = false;
                task.bFinished = true;
            }
        }

        if (!task.bFinished && !task.bCanceled && task.bOK)
        {
            std::lock_guard lock(mutexTQ);
            taskQueue.push(std::move(task));
            atomicTQLength.fetch_add(1);
        }
        else
        {
            std::lock_guard lock(mutexFinishedTaskList);
            finishedTasks.push_back(task);
        }
    }

    void FileIOThread::PushTask(FileIOTask && inTask)
    {
        std::lock_guard lock(mutexTQ);
        taskQueue.push(std::move(inTask));
        atomicTQLength.fetch_add(1);
        atomicAwakeSignal.store(true);
        atomicAwakeSignal.notify_all();
    }
}
