#pragma once
#include <functional>
#include <future>

#include "Core/ThreadTypes.h"

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
            : func(std::move(inTask.func)), arg(inTask.arg), assignThread(std::move(inTask.assignThread)), promise(std::move(inTask.promise)) {}
        Task& operator=(Task&& inTask)
        {
            if (&inTask == this)
                return *this;
            func = std::move(inTask.func);
            arg = inTask.arg;
            assignThread = std::move(inTask.assignThread);
            promise = std::move(inTask.promise);
            inTask.Reset();
            return *this;
        }

        void Reset()
        {
            func = nullptr;
            arg = nullptr;
            assignThread = EThreadType::UnknownThread;
        }
        
        TaskFuncType func;
        TaskArgType  arg;
        
        EThreadType assignThread{EThreadType::UnknownThread};
        std::promise<void> promise;

        Task(std::function<void(void*)> inFunc, void* inArg, EThreadType inAssignThread):
            func(inFunc), arg(inArg), assignThread(inAssignThread) {}
        Task& GetFuture(std::future<void>& outFuture) { outFuture = std::move(promise.get_future()); return *this;}
    };
    
}
