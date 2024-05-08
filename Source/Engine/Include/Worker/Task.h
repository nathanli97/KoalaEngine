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
    
    struct TaskMetaData
    {
        TaskMetaData() = default;
        TaskFuncType func;
        TaskArgType  arg;
        EThreadType assignThread{EThreadType::UnknownThread};
        std::atomic<uint8_t> taskPriority{(uint8_t)ETaskPriority::Normal};
    };
    
    struct Task
    {
        Task() = default;
        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;
        Task(Task&& inTask) noexcept
            : data(inTask.data) {}
        Task& operator=(Task&& inTask)
        {
            if (&inTask == this)
                return *this;
            data = inTask.data;
            inTask.Reset();
            return *this;
        }

        FORCEINLINE void Reset()
        {
            data = nullptr;
        }
        
        TaskMetaData *data{nullptr};

        Task(TaskMetaData* inData):
            data(inData) {}
    };
    
}
