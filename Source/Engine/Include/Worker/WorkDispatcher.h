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

#include "Worker.h"
#include "Core/SingletonInterface.h"
#include "Core/ThreadTypes.h"
#include "Core/ThreadedModule.h"
#include "TSContainer/QueueTS.h"

namespace Koala
{
    class WorkDispatcher: public IThreadedModule
    {
    private:
        struct Task
        {
            std::function<void(void*)> func;
            void* arg{nullptr};
            EThreadType assignThread{EThreadType::UnknownThread};

            Task(std::function<void(void*)> inFunc, void* inArg, EThreadType inAssignThread):
                func(inFunc), arg(inArg), assignThread(inAssignThread) {}
        };
    public:
        KOALA_IMPLEMENT_SINGLETON(WorkDispatcher)

        typedef std::function<void(void*)> TaskFuncType;
        typedef std::pair<TaskFuncType, void*> TaskPairType;
        
        void Run() override;
        bool Initialize_MainThread() override;
        bool Shutdown_MainThread() override;
        void Tick(float delta_time) override;

        template <typename Lambda, typename Arg = void>
        void AddTask(Lambda&& inTask, Arg* inArg = nullptr, EThreadType inAssignThread)
        {
            pendingAddTasks.Push(Task(inTask, inArg, inAssignThread));
        }
    private:
        QueueTS<Task> pendingAddTasks;
        // Map for storing tasks which will dispatched to MainThread/RenderThread/RHIThread
        std::unordered_map<EThreadType, QueueTS<TaskPairType>> taskRemap;
    };
}
