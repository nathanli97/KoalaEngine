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

namespace Koala
{
    class WorkDispatcher: public IThreadedModule
    {
    public:
        typedef std::function<void(void*)> TaskFuncType;
        typedef std::pair<TaskFuncType, void*> TaskPairType;
    private:
        struct Task
        {
            TaskPairType task;
            // std::promise<void> promise;
            EThreadType assignThread{EThreadType::UnknownThread};

            Task(std::function<void(void*)> inFunc, void* inArg, EThreadType inAssignThread):
                task(std::make_pair(inFunc, inArg)), assignThread(inAssignThread) {}
            Task() = default;
            // Task& GetFuture(std::future<void>& outFuture) { outFuture = promise.get_future(); return *this;}
        };
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
        QueueTS<Task> pendingAddTasks;
        
        std::queue<TaskPairType> taskListMainThread;
        std::mutex mutexTaskMainThread;
        std::queue<TaskPairType> taskListRenderThread;
        std::mutex mutexTaskRenderThread;
        std::queue<TaskPairType> taskListRHIThread;
        std::mutex mutexTaskRHIThread;
        std::queue<TaskPairType> taskListWorkerThread;
        std::mutex mutexTaskWorkerThread;

        void ProcessNewWorkerTask(Task &task, bool bInEngineIsShutdowning);

        std::vector<Worker*>       workerThreads;
        std::mutex                mutexWorkerThreads;
    };
}
