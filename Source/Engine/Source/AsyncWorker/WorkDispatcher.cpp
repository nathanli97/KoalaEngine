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

#include "AsyncWorker/WorkDispatcher.h"

#include <iostream>

#include "CPUProfiler.h"
#include "KoalaEngine.h"
#include "Core/ThreadManager.h"

#define PROCESS_LOCAL_TASKS(LIST, MUTEX) { \
    std::queue<TaskPtr> localQueue; \
    { \
        std::lock_guard lock(MUTEX); localQueue.swap(LIST); \
    } \
    while (!localQueue.empty()) \
    { \
        TaskPtr t = std::move(localQueue.front()); \
        t->func(t->arg); \
        finishedNonWorkerTasks.Push(std::move(t)); \
    } \
    }
namespace Koala::AsyncWorker
{
    static Logger logger("WorkDispatcher");
    void WorkDispatcher::Run()
    {
        while (true)
        {
            const bool bEngineIsShutdowning = KoalaEngine::Get().IsEngineExitRequested();

            // How many ms we should wait to reduce CPU load
            uint32_t waitTime = 1;
            
            ProcessFinishedTasks();
            DispatchWorkerTasks();
            
            TaskPtr localTask;
            if (bEngineIsShutdowning)
            {
                if (!CheckOutTaskByPriority(localTask))
                    return;
            }
            else
            {
                if (!CheckOutTaskByPriority(localTask))
                {
                    if (workerTasks.size() < 10)
                        std::this_thread::sleep_for(std::chrono::nanoseconds(10));
                    continue;
                }
            }

            if (CheckAndHandleTaskCancel(localTask))
                continue;

            // Are we have a valid task?
            auto assignThread = localTask->assignThread;
            if (assignThread != EThreadType::UnknownThread)
            {
                switch (assignThread)
                {
                    case EThreadType::MainThread:
                    {
                        std::scoped_lock lock(mutexTaskMainThread);
                        taskListMainThread.emplace(std::move(localTask));
                        break;
                    }
                    case EThreadType::RenderThread:
                    {
                        std::scoped_lock lock(mutexTaskRenderThread);
                        taskListRenderThread.emplace(std::move(localTask));
                        break;
                    }
                    case EThreadType::RHIThread:
                    {
                        std::scoped_lock lock(mutexTaskRHIThread);
                        taskListRHIThread.emplace(std::move(localTask));
                        break;
                    }
                    case EThreadType::WorkerThread:
                    {
                        ProcessNewWorkerTask(std::move(localTask), bEngineIsShutdowning);
                        break;
                    }
                    default: break;
                }
            }

            
        }
    }
    bool WorkDispatcher::CheckOutTaskByPriority(TaskPtr &out)
    {
#define KOALA_CONDITIONAL_CHECKOUT_PRIORITY(Priority) {sum += (uint8_t)ETaskPriorityWeight::Priority;} if (sum > luckyValue) {if (pendingAddTasks[(uint8_t)(ETaskPriority::Priority)].TryPop(out)) return true;}
        constexpr auto totalTicket = (uint8_t)ETaskPriorityWeight::TaskPriorityWeightSum;
        uint8_t luckyValue = Random() % totalTicket;
        int sum = 0;
        KOALA_CONDITIONAL_CHECKOUT_PRIORITY(Highest)
        KOALA_CONDITIONAL_CHECKOUT_PRIORITY(High)
        KOALA_CONDITIONAL_CHECKOUT_PRIORITY(Normal)
        KOALA_CONDITIONAL_CHECKOUT_PRIORITY(Low)
        KOALA_CONDITIONAL_CHECKOUT_PRIORITY(Lowest)
        return false;
#undef KOALA_CONDITIONAL_PROCESS_PRIORITY
    }

    void WorkDispatcher::ProcessNewWorkerTask(TaskPtr &&task, bool bInEngineIsShutdowning)
    {
        if (bInEngineIsShutdowning)
        {
            std::scoped_lock lock(mutexTaskMainThread);
            taskListMainThread.emplace(std::move(task));
        } else
        {
            int p = (uint8_t)task->taskPriority;
            workerTasks.push(std::move(task));
        }
    }

    void WorkDispatcher::DispatchWorkerTasks()
    {
        SCOPED_CPU_MARKER(Colors::Green, "WorkDispatcher::DispatchWorkerTasks")

        while (!workerTasks.empty() && CheckAndHandleTaskCancel(workerTasks.front()))
        {
            workerTasks.pop();
        }
        
        if (workerTasks.empty())
        {
            return;
        }
        
        for (auto worker: workerThreads)
        {
            if (workerTasks.empty())
            {
                return;
            }
            if (worker->IsIdle())
            {
                worker->AssignTask(std::move(workerTasks.front()));
                worker->Execute();
                workerTasks.pop();
            }
        }
    }

    void WorkDispatcher::ProcessFinishedTasks()
    {
        SCOPED_CPU_MARKER(Colors::Blue, "WorkDispatcher::ProcessFinishedTasks")
        std::forward_list<Worker*> finishedWorkers;
        for (auto worker: workerThreads)
        {
            if (worker->IsFinished())
            {
                finishedWorkers.push_front(worker);
            }
        }

        std::forward_list<TaskPtr> finishedTasks;
        for (auto worker: finishedWorkers)
        {
            finishedTasks.push_front(worker->FinishTask());
        }

        TaskPtr t;
        while (finishedNonWorkerTasks.TryPop(t))
        {
            finishedTasks.push_front(std::move(t));
        }
        for (auto task: finishedTasks)
        {
            task->status.store(ETaskStatus::Completed, std::memory_order::relaxed);
            if (task->bHasWaiter.load(std::memory_order::relaxed))
            {
                std::unique_lock lock(task->mutex);
                task->cvWaitForFinishedOrCanceled.notify_all();
            }
        }

    }

    bool WorkDispatcher::CheckAndHandleTaskCancel(TaskPtr &task)
    {
        SCOPED_CPU_MARKER(Colors::Purple, "WorkDispatcher::CheckAndHandleTaskCancel")
        if (task->RequiredShouldCancel())
        {
            task->status.store(ETaskStatus::Canceled, std::memory_order::release);
            if (task->bHasWaiter.load(std::memory_order::relaxed))
            {
                std::unique_lock lock(task->mutex);
                task->cvWaitForFinishedOrCanceled.notify_all();
            }
            return true;
        }
        return false;
    }

    WorkDispatcher::WorkDispatcher()
        :numWorkerThreads(std::thread::hardware_concurrency())
    {
        if (numWorkerThreads > 6)
            numWorkerThreads -= 4;
        else
        {
            numWorkerThreads = 2;
        }
    }

    bool WorkDispatcher::Initialize_MainThread()
    {
        auto nCores = numWorkerThreads;

        workerThreads.resize(nCores);
        
        for (Worker* &worker: workerThreads)
        {
            worker = new Worker();
            ThreadManager::Get().CreateThreadManaged(worker);
            worker->WaitForThreadCreated();
        }

        logger.info("Created {} worker thread(s)", nCores);
        return true;
    }

    void WorkDispatcher::Tick_MainThread(float)
    {
        PROCESS_LOCAL_TASKS(taskListMainThread, mutexTaskMainThread)
    }

    void WorkDispatcher::Tick_RenderThread()
    {
        PROCESS_LOCAL_TASKS(taskListRenderThread, mutexTaskRenderThread)
    }

    void WorkDispatcher::Tick_RHIThread()
    {
        PROCESS_LOCAL_TASKS(taskListRHIThread, mutexTaskRHIThread)
    }
    
    bool WorkDispatcher::Shutdown_MainThread()
    {
        // Do not delete worker thread. ThreadManager will release all IThread* object when it is exited.
        for (auto worker: workerThreads)
        {
            worker->Exit();
        }
        return true;
    }



}
