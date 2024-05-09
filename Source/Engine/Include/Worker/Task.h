#pragma once
#include <functional>
#include <future>
#include <shared_mutex>

#include "Core/ThreadTypes.h"
namespace Koala
{
    class WorkDispatcher;
    enum class ETaskPriority: uint8_t
    {
        TaskPriorityMaximum = 5, // Not a valid priority, just maximum value of enum

        Highest     = 4,
        High        = 3,
        Normal      = 2,
        Low         = 1,
        Lowest      = 0,
    };

    enum class ETaskPriorityWeight: uint8_t
    {
        // The following value stands for the weight when scheduling
        Highest     = 6,  // 30%
        High        = 5,  // 25%
        Normal      = 4,  // 20%
        Low         = 3,  // 15%
        Lowest      = 2,  // 10%

        TaskPriorityWeightSum = Lowest + Low + Normal + High + Highest,
    };
}

namespace Koala::Worker
{
    class Worker;
    
    typedef std::function<void(void*)> TaskFuncType;
    typedef void* TaskArgType;
    
    enum class ETaskStatus: uint8_t
    {
        Ready,
        Running,
        Completed,
        Canceled
    };
    class Task
    {
    public:
        friend class Worker;
        friend class WorkDispatcher;
        Task(TaskFuncType &&inFunc, void* inArg, EThreadType inAssignThread, ETaskPriority inDefaultPriority):
            func(std::move(inFunc)), arg(inArg), assignThread(inAssignThread), taskPriority(inDefaultPriority) {}
        Task(const TaskFuncType &inFunc, void* inArg, EThreadType inAssignThread, ETaskPriority inDefaultPriority):
            func(inFunc), arg(inArg), assignThread(inAssignThread), taskPriority(inDefaultPriority) {}
        Task() = default;
        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;
        Task(Task&& inTask) noexcept
            : func(std::move(inTask.func)), arg(inTask.arg), assignThread(inTask.assignThread), taskPriority(std::move(inTask.taskPriority)) {}
        Task& operator=(Task&& inTask) noexcept
        {
            if (&inTask == this)
                return *this;
            func = std::move(inTask.func);
            inTask.func = nullptr;
            arg = inTask.arg;
            assignThread = inTask.assignThread;
            taskPriority = inTask.taskPriority;
            return *this;
        }

        void Wait()
        {
            bHasWaiter.store(true, std::memory_order::release);
            std::unique_lock lock(mutex);
            while (status.load() != ETaskStatus::Completed && status.load() != ETaskStatus::Canceled)
                cvWaitForFinishedOrCanceled.wait(lock);
        }

        void WaitFor(uint32_t inMS)
        {
            bHasWaiter.store(true, std::memory_order::release);
            std::unique_lock lock(mutex);
            if (status.load() != ETaskStatus::Completed && status.load() != ETaskStatus::Canceled)
                cvWaitForFinishedOrCanceled.wait_for(lock, std::chrono::milliseconds(inMS));
            bHasWaiter.store(false, std::memory_order::relaxed);
        }

        bool IsCompleted() const
        {
            return status.load() == ETaskStatus::Completed;
        }

        void Cancel()
        {
            auto currentStatus = status.load();
            if (currentStatus == ETaskStatus::Completed || currentStatus == ETaskStatus::Canceled)
                return;
            bShouldCancel.store(true, std::memory_order::relaxed);
        }

        bool IsCanceled() const
        {
            return status.load(std::memory_order::relaxed) == ETaskStatus::Canceled;
        }

        bool IsFinished() const
        {
            auto currentStatus = status.load();
            return currentStatus == ETaskStatus::Completed || currentStatus == ETaskStatus::Canceled;
        }

        bool ShouldCancel() const
        {
            return bShouldCancel.load(std::memory_order::relaxed);
        }
    private:
        TaskFuncType func;
        TaskArgType  arg;
        std::atomic<ETaskStatus> status;
        
        std::condition_variable  cvWaitForFinishedOrCanceled;
        mutable std::mutex               mutex;
        mutable std::atomic<bool>        bHasWaiter{false};
        std::atomic<bool>                bShouldCancel{false};
        
        EThreadType assignThread{EThreadType::UnknownThread};
        ETaskPriority taskPriority = ETaskPriority::Normal;
    };
}

namespace Koala
{
    typedef std::shared_ptr<Worker::Task> TaskPtr;
}
