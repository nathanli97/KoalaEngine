#pragma once
#include <functional>
#include <future>

#include "Core/ThreadTypes.h"
namespace Koala
{
    enum class ETaskPriority: uint8_t
    {
        Highest     = 4,
        High        = 3,
        Normal      = 2,
        Low         = 1,
        Lowest      = 0
    };
}

namespace Koala::Worker
{
    typedef std::function<void(void*)> TaskFuncType;
    typedef void* TaskArgType;
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
        
        EThreadType assignThread{EThreadType::UnknownThread};
        ETaskPriority taskPriority = ETaskPriority::Normal;

        Task(TaskFuncType &&inFunc, void* inArg, EThreadType inAssignThread, ETaskPriority inDefaultPriority):
            func(std::move(inFunc)), arg(inArg), assignThread(inAssignThread), taskPriority(inDefaultPriority) {}
        Task(const TaskFuncType &inFunc, void* inArg, EThreadType inAssignThread, ETaskPriority inDefaultPriority):
            func(inFunc), arg(inArg), assignThread(inAssignThread), taskPriority(inDefaultPriority) {}
    };
    
}
