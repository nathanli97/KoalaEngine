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
#include "Core/ThreadInterface.h"
#include "Definations.h"
#include "Core/Check.h"
#include "Task.h"

namespace Koala::AsyncWorker
{
    class WorkDispatcher;

    enum class EWorkerStatus: uint8_t
    {
        Uninitialized = 0,
        Idle,        // No task now. Can SetTask() and Execute(), etc.
        Ready,       // After Execute(). But this task is not executed yet.
        Busy,        // Task is running.
        Finished,     // Task is finished. Need call Reset() reset this worker to Idle status.
        Exited
    };
    
    // The workers can be only managed by one single dispatcher thread.
    class Worker: public IThread
    {
    public:
        Worker() = default;
        ~Worker() override {}
        friend class WorkDispatcher;
        void Run() override;
    protected:
        // Reset from Finished to Idle
        FORCEINLINE TaskPtr&& FinishTask()
        {
            std::unique_lock lock(mutex);
            ensure(status.load(std::memory_order::acquire) == EWorkerStatus::Finished, "Only Finished worker can be reset to Idle");
            status.store(EWorkerStatus::Idle, std::memory_order::release);
            return std::move(task);
        }
        // Set new task. Worker status must be Idle. Can only be called when status==Idle.
        FORCEINLINE void AssignTask(TaskPtr&& inTask)
        {
            ensure(status.load(std::memory_order::acquire) == EWorkerStatus::Idle, "Only Idle worker can AssignTask. Do you forgot to call Reset?");
            task = std::move(inTask);
        }
        
        // Execute the task. This will turn status into Ready
        FORCEINLINE void Execute()
        {
            ensure(status.load(std::memory_order::acquire) == EWorkerStatus::Idle, "Only Idle worker can be Execute. Do you forgot to call Reset?");
            check(task->func.operator bool(), "Task is empty??");
            
            if (task->func)
            {
                std::unique_lock lock(mutex);
                // We need to ensure 'cvWorkerWaitTask.notify_one()' is happened-after 'status.store'!
                status.store(EWorkerStatus::Ready, std::memory_order::release);
                lock.unlock();
                cvWorkerWaitNewTask.notify_one();
            }
        }
        FORCEINLINE bool IsFinished()
        {
            return status.load(std::memory_order::seq_cst) == EWorkerStatus::Finished;
        }
        FORCEINLINE bool IsIdle()
        {
            return status.load(std::memory_order::seq_cst) == EWorkerStatus::Idle;
        }
        FORCEINLINE void Exit()
        {
            bShouldExit.store(true, std::memory_order::relaxed);
        }

        FORCEINLINE void WaitForThreadCreated()
        {
            std::unique_lock lock(mutex);
            
            while (status.load() == EWorkerStatus::Uninitialized)
            {
                cvWorkerThreadCreated.wait(lock);
            }
        }
    
    
    private:
        std::atomic<EWorkerStatus> status{EWorkerStatus::Uninitialized};
        std::atomic<bool>          bShouldExit{false};
        
        // Task can only be updated when status==Idle!
        std::shared_ptr<Task> task;
        
        std::condition_variable    cvWorkerWaitNewTask;
        std::condition_variable    cvWorkerThreadCreated;

        std::mutex                 mutex;
    };
}
