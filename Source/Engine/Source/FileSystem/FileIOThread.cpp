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
            while (atomicHasPendingTask.load() == false)
                atomicHasPendingTask.wait(true);
            DoIOTask();
        }
    }

    void FileIOThread::ShutdownIOThread()
    {
        atomicShouldShutdown.store(true);
        atomicShouldShutdown.notify_all();
    }


    void FileIOThread::DoWork()
    {
        if (!taskQueue.empty())
            return;

        auto task = taskQueue.front();
        taskQueue.pop();
        
        
        if (task.remainingSize != 0 && task.bufferStart)
        {
            int64_t size = 0;
            char * buffer = static_cast<char*>(task.bufferStart) + task.offset;
            
            FileHandle handle = task.handle;
            
            int64_t blocks = std::min(task.remainingSize / BlockSize, FilePriorityToBlockNum(task.handle->priority));
            blocks = std::min(blocks, MaxContinuousIOWorkBlocks);
            
            if (blocks == 0)
                blocks = 1;

            if (bIsReadThread)
            {
                for (auto i = 0; i < blocks; ++i)
                {
                    int64_t blockSize = std::min(BlockSize, task.remainingSize);

                    handle->fileStream.read(buffer + size, blockSize);
                    size += handle->fileStream.gcount();
                }
            }
            else
            {
                for (auto i = 0; i < blocks; ++i)
                {
                    int64_t blockSize = std::min(BlockSize, task.remainingSize);

                    handle->fileStream.write(buffer + size, blockSize);
                    size += handle->fileStream.gcount();
                }
            }
            
            task.offset += size;
            
            if (!bIsReadThread || !handle->IsEOF() && handle->IsValid())
                task.remainingSize -= size;
            else
                task.remainingSize = 0;
        }
    }
}