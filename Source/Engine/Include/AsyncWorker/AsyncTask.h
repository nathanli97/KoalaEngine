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
#include "TaskSet.h"
#include "WorkDispatcher.h"

namespace Koala
{
    template <typename Lambda, typename Arg = nullptr_t>
    TaskPtr AsyncTask(Lambda&& inTask, Arg inArg = nullptr, ETaskPriority inTaskPriority = ETaskPriority::Normal, EThreadType inAssignThread = EThreadType::WorkerThread)
    {
        return AsyncWorker::WorkDispatcher::Get().EnqueueNewTask(std::forward<Lambda>(inTask), inArg, inTaskPriority, inAssignThread);
    }

    template <typename Lambda>
    TaskSetPtr Async(Lambda&& inTask, size_t numOfTasks, void* inMem = nullptr, ETaskPriority inTaskPriority = ETaskPriority::Normal, EThreadType inAssignThread = EThreadType::WorkerThread)
    {
        TaskSetPtr ptr = std::make_shared<AsyncWorker::TaskSet>(numOfTasks);
        for (size_t index = 0; index < numOfTasks; index++)
        {
            ptr->tasks[index] = AsyncWorker::WorkDispatcher::Get().EnqueueNewTask([index, inTask](void* mem)
            {
                inTask(mem, index);
            }, inMem, inTaskPriority, inAssignThread);
        }
    }
}