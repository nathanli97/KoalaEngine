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

#include "FileSystem/IOManager.h"

#include "ConsoleVariable.h"
#include "Core/ThreadManager.h"

namespace Koala::FileIO
{
    Logger logger("IO");
    static TConsoleVariable<uint32_t> CVarNumReadThreads(
        "io.readthreads",
        4,
        "Number of File IO Read Threads"
    );
    static TConsoleVariable<uint32_t> CVarNumWriteThreads(
        "io.writethreads",
        1,
        "Number of File IO Write Threads"
    );

    IOTaskPtr IOManager::NewReadRequest(FileHandle inFileHandle, size_t offset, size_t size, void *dstBuffer)
    {
        
    }

    IOTaskPtr IOManager::NewWriteRequest(FileHandle inFileHandle, size_t offset, size_t size, void *srcBuffer)
    {
        
    }

    void IOManager::Run()
    {
    }

    bool IOManager::Initialize_MainThread()
    {
        logger.info("Initialing FileIO System with {} IOReadThread(s), {} IOWriteThread(s)", CVarNumReadThreads.Get());
        readThreads.resize(CVarNumReadThreads.Get());
        writeThreads.resize(CVarNumWriteThreads.Get());

        for (AsyncWorker::Worker* worker: readThreads)
        {
            worker = new AsyncWorker::Worker();
            ThreadManager::Get().CreateThreadManaged(worker);
            worker->WaitForThreadCreated();
        }

        for (AsyncWorker::Worker* worker: writeThreads)
        {
            worker = new AsyncWorker::Worker();
            ThreadManager::Get().CreateThreadManaged(worker);
            worker->WaitForThreadCreated();
        }

        ioReadTasksPerThread.resize(CVarNumReadThreads.Get());
        ioWriteTasksPerThread.resize(CVarNumWriteThreads.Get());

        return true;
    }

    bool IOManager::Shutdown_MainThread()
    {
        for (AsyncWorker::Worker* worker: readThreads)
        {
            worker->Exit();
        }

        for (AsyncWorker::Worker* worker: writeThreads)
        {
            worker->Exit();
        }

        return true;
    }

    void IOManager::Tick_MainThread(float deltaTime) {}

    void IOManager::DispatchIOWork()
    {
    }

    void IOManager::ProcessFinishedIOWork()
    {
    }
}
