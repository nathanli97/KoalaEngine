#pragma once
#include <functional>
#include <future>

#include "Core/ThreadTypes.h"
namespace Koala
{
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
    typedef std::function<void(void*)> TaskFuncType;
    typedef void* TaskArgType;

    struct Task;
    enum class ETaskResolvedReason: uint8_t
    {
        Unknown,
        Finished,
        Canceled
    };
    struct TaskTracker
    {
        explicit TaskTracker(uint32_t inTaskCount = 1): taskCount(inTaskCount) {}
        ~TaskTracker() {}
        TaskTracker(const TaskTracker&) = delete;
        TaskTracker(TaskTracker&&) = delete;
        std::atomic<uint32_t>         resolvedCount;         // Task Resolved means task has been completed or canceled
        uint32_t                      taskCount;
        std::atomic<bool>             bHasWaiter;
        std::condition_variable       cvNotifyTaskResolved;
        ETaskResolvedReason           taskResolvedReason{ETaskResolvedReason::Unknown};
    };

    struct Task
    {
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
        
        
        TaskFuncType func;
        TaskArgType  arg;
        TaskTracker  *tracker{nullptr};
        
        EThreadType assignThread{EThreadType::UnknownThread};
        ETaskPriority taskPriority = ETaskPriority::Normal;

        Task(TaskFuncType &&inFunc, void* inArg, EThreadType inAssignThread, ETaskPriority inDefaultPriority):
            func(std::move(inFunc)), arg(inArg), assignThread(inAssignThread), taskPriority(inDefaultPriority) {}
        Task(const TaskFuncType &inFunc, void* inArg, EThreadType inAssignThread, ETaskPriority inDefaultPriority):
            func(inFunc), arg(inArg), assignThread(inAssignThread), taskPriority(inDefaultPriority) {}
    };

    
}
