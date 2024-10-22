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
#include "Worker.h"
#include "Core/SingletonInterface.h"
#include "Core/ThreadTypes.h"
#include "Core/ThreadedModule.h"
#include "TSContainer/QueueTS.h"
#include "Task.h"

namespace Koala::AsyncWorker
{
    class WorkDispatcher: public IThreadedModule
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(WorkDispatcher)
        WorkDispatcher();
        void Run() override;
        bool Initialize_MainThread() override;
        bool Shutdown_MainThread() override;
        void Tick_MainThread(float delta_time) override;
        void Tick_RenderThread();
        void Tick_RHIThread();

        template <typename Lambda, typename Arg = nullptr_t>
        TaskPtr EnqueueNewTask(Lambda&& inTask, Arg inArg = nullptr, ETaskPriority inTaskPriority = ETaskPriority::Normal, EThreadType inAssignThread = EThreadType::WorkerThread)
        {
            TaskPtr taskPtr = std::make_shared<Task>(std::forward<Lambda>(inTask), inArg, inAssignThread, inTaskPriority);
            pendingAddTasks[(uint8_t)inTaskPriority].Push(taskPtr);
            return taskPtr;
        }

        FORCEINLINE_DEBUGABLE size_t GetNumWorkerThreads() const { return numWorkerThreads;}
    private:
        bool CheckOutTaskByPriority(TaskPtr &out);
        QueueTS<TaskPtr> pendingAddTasks[(uint8_t)ETaskPriority::TaskPriorityMaximum];
        
        std::queue<TaskPtr> taskListMainThread;
        std::mutex mutexTaskMainThread;
        std::queue<TaskPtr> taskListRenderThread;
        std::mutex mutexTaskRenderThread;
        std::queue<TaskPtr> taskListRHIThread;
        std::mutex mutexTaskRHIThread;

        void ProcessNewWorkerTask(TaskPtr &&task, bool bInEngineIsShutdowning);
        void DispatchWorkerTasks();
        void ProcessFinishedTasks();
        bool CheckAndHandleTaskCancel(TaskPtr& task);

        std::vector<Worker*>       workerThreads;
        std::mutex                mutexWorkerThreads;

        std::queue<TaskPtr> workerTasks;
        QueueTS<TaskPtr> finishedNonWorkerTasks;

        size_t numWorkerThreads{0};
    };
}
