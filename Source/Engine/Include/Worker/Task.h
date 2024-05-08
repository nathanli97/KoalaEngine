#pragma once
#include <functional>
#include <future>

#include "Core/ThreadTypes.h"

namespace Koala
{
    enum class ETaskPriority: uint8_t
    {
        Highest     = 255,
        High        = 192,
        AboveNormal = 144,
        Normal      = 128,
        BelowNormal = 112,
        Low         = 64,
        Lowest      = 16
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
            : func(std::move(inTask.func)), arg(inTask.arg), assignThread(std::move(inTask.assignThread)), taskPriority(std::move(inTask.taskPriority)) {}
        Task& operator=(Task&& inTask)
        {
            if (&inTask == this)
                return *this;
            func = std::move(inTask.func);
            arg = inTask.arg;
            assignThread = std::move(inTask.assignThread);
            taskPriority = std::move(inTask.taskPriority);
            inTask.Reset();
            return *this;
        }

        FORCEINLINE void Reset()
        {
            func = nullptr;
            arg = nullptr;
            assignThread = EThreadType::UnknownThread;
            taskPriority = (uint8_t)ETaskPriority::Normal;
        }
        
        TaskFuncType func;
        TaskArgType  arg;

        // Metadata -- Only used in Dispatcher thread: Will be strikeout from Worker
        // TODO: Strikeout metadata when metadata takes too much memory
        // For now, the following metadata only used 2 Bytes of storage
        // TODO: taskPriority is unused for now
        EThreadType assignThread{EThreadType::UnknownThread};
        uint8_t taskPriority{(uint8_t)ETaskPriority::Normal};

        Task(std::function<void(void*)> inFunc, void* inArg, EThreadType inAssignThread, ETaskPriority inTaskPriority):
            func(inFunc), arg(inArg), assignThread(inAssignThread), taskPriority((uint8_t)inTaskPriority) {}
    };
    
}
