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
    constexpr uint32_t IOThreadMaxQueueLength = 500;
    Logger logger("FileIOManager");
    bool FileIOManager::Initialize_MainThread()
    {
        auto numCPUCores = std::thread::hardware_concurrency();

        numReadThreads = std::min(numCPUCores, numReadThreads);
        numWriteThreads = std::min(numCPUCores, numWriteThreads);

        logger.info("Creating IO Threads: {} readThreads, {} writeThreads", numReadThreads, numWriteThreads);
        for (uint32_t i = 0; i < numReadThreads; i++)
        {
            auto handle = new FileIOThread(true);
            readThreadHandles.push_back(handle);
            ThreadManager::Get().CreateThreadManaged(handle);
        }

        for (uint32_t i = 0; i < numWriteThreads; i++)
        {
            auto handle = new FileIOThread(false);
            writeThreadHandles.push_back(handle);
            ThreadManager::Get().CreateThreadManaged(handle);
        }
        return true;
    }

    bool FileIOManager::Shutdown_MainThread()
    {
        logger.info("Shutdowning IO Threads...");
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
        // Process finished tasks
        for (auto threadHandle: readThreadHandles)
        {
            TickFileIOThread(threadHandle);
        }

        for (auto threadHandle: writeThreadHandles)
        {
            TickFileIOThread(threadHandle);
        }

        // Process new tasks
        if (remainingReadTasks.empty() && remainingWriteTasks.empty())
            return;

        // Consider remaining tasks
        TickRemainingIOTasks(remainingReadTasks, readThreadHandles);
        TickRemainingIOTasks(remainingWriteTasks, writeThreadHandles);
    }

    void FileIOManager::TickFileIOThread(IThread* threadHandle)
    {
        FileIOThread* thread = dynamic_cast<FileIOThread*> (threadHandle);
        std::list<FileIOTask> finishedTasks;

        thread->GetFinishedTasks(finishedTasks);

        for (auto &task: finishedTasks)
        {
            if (task.callback)
            {
                task.callback(task.bOK, task.performedSize, task.bufferStart);
            }
        }
    }

    void FileIOManager::TickRemainingIOTasks(std::queue<FileIOTask> &taskQueue, const std::vector<IThread*> &threadHandles)
    {
        while (!taskQueue.empty())
        {
            auto task = taskQueue.front();
            auto fileName = task.handle->fileName;
            if (task.handle->currWorkingIOThread)
            {
                FileIOThread* thread = dynamic_cast<FileIOThread*> (task.handle->currWorkingIOThread);
                thread->PushTask(std::move(task));
            }
            else
            {
                FileIOThread* minThread{nullptr};
                for (auto threadHandle: threadHandles)
                {
                    FileIOThread* thread = dynamic_cast<FileIOThread*> (threadHandle);
                    if (!minThread || thread->GetQueueLength() < minThread->GetQueueLength())
                    {
                        minThread = thread;
                    }
                }

                if (minThread->GetQueueLength() > IOThreadMaxQueueLength)
                    break;

                minThread->PushTask(std::move(task));
            }

            taskQueue.pop();
        }
    }

    void FileIOManager::RequestReadFileAsync(FileHandle inHandle, size_t offset, size_t size, void *buffer,
        FileIOCallback callback)
    {
        FileIOTask task;
        task.handle = std::move(inHandle);
        task.offset = offset;
        task.remainingSize = size;
        task.bufferStart = buffer;
        task.callback = std::move(callback);

        remainingReadTasks.push(std::move(task));
    }

    void FileIOManager::RequestWriteFileAsync(FileHandle inHandle, size_t offset, size_t size, const void *buffer,
        FileIOCallback callback)
    {
        FileIOTask task;
        task.handle = std::move(inHandle);
        task.offset = offset;
        task.remainingSize = size;
        task.bufferStart = const_cast<void *>(buffer);
        task.callback = std::move(callback);

        remainingWriteTasks.push(std::move(task));
    }
}
