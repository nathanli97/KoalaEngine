// Copyright 2023 Li Xingru
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the “Software”), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once
#include <future>
#include <unordered_set>

#include "Worker.h"
#include "Core/SingletonInterface.h"
#include "Core/ThreadTypes.h"
#include "Core/ThreadedModule.h"
#include "TSContainer/QueueTS.h"
#include "Task.h"
namespace Koala
{
    class WorkDispatcher: public IThreadedModule
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(WorkDispatcher)
        void Run() override;
        bool Initialize_MainThread() override;
        bool Shutdown_MainThread() override;
        void Tick_MainThread(float delta_time) override;
        void Tick_RenderThread();
        void Tick_RHIThread();

        template <typename Lambda, typename Arg = void>
        void AddTask(Lambda&& inTask, Arg* inArg = nullptr, EThreadType inAssignThread = EThreadType::WorkerThread)
        {
            pendingAddTasks.Push(Task(inTask, inArg, inAssignThread));
        }
    private:
        QueueTS<Worker::Task> pendingAddTasks;
        
        std::queue<Worker::Task> taskListMainThread;
        std::mutex mutexTaskMainThread;
        std::queue<Worker::Task> taskListRenderThread;
        std::mutex mutexTaskRenderThread;
        std::queue<Worker::Task> taskListRHIThread;
        std::mutex mutexTaskRHIThread;
        std::queue<Worker::Task> taskListWorkerThread;
        std::mutex mutexTaskWorkerThread;

        void ProcessNewWorkerTask(Worker::Task &&task, bool bInEngineIsShutdowning);
        void DispatchWorkerTasks();
        void ProcessFinishedTasks();

        std::vector<Worker::Worker*>       workerThreads;
        std::mutex                mutexWorkerThreads;

        QueueTS<Worker::Task> undispatchedWorkerTasks;
        QueueTS<Worker::Task> finishedNonWorkerTasks;
    };
}