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

#pragma once

#include <shared_mutex>
#include <thread>
#include <unordered_map>

#include "Check.h"
#include "SingletonInterface.h"
#include "ThreadInterface.h"
#include "ThreadTypes.h"

namespace Koala
{
    struct ThreadTLS
    {
        static thread_local std::thread::id threadId;
        static thread_local EThreadType threadType;
        static thread_local uint32_t ThreadIndexOfType;
        static void Initialize(EThreadType inThreadType, uint32_t inThreadIndexOfType = 0);
    };
    FORCEINLINE bool IsInMainThread()
    {
        return ThreadTLS::threadType == EThreadType::MainThread;
    }
    FORCEINLINE bool IsInRenderThread()
    {
        return ThreadTLS::threadType == EThreadType::RenderThread;
    }
    class ThreadManager final: public ISingleton
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(ThreadManager)
        void CreateThread(IThread* inThread, bool bShouldDeleteThreadObjectWhenShutdown = false)
        {
            std::lock_guard lock(mutexForThreadList);
            threads.emplace_back(
                [=, this]
                {
                    inThread->Run();
                });
            if (bShouldDeleteThreadObjectWhenShutdown)
            {
                threadObjects.push_back(inThread);
            }
        }
        template <typename Callable>
        void CreateThread(Callable inFunc) requires std::invocable<Callable>
        {
            std::lock_guard lock(mutexForThreadList);
            threads.emplace_back(
                [=, this]
                {
                    inFunc();
                });
        }
        ~ThreadManager()
        {
            for (auto & thread: threads)
            {
                if (thread.joinable())
                    thread.join();
            }
            threads.clear();
            for (auto object: threadObjects)
            {
                delete object;
            }
        }
    private:
        std::list<std::thread> threads;
        std::list<IThread*>    threadObjects;
        mutable std::mutex mutexForThreadList;
        mutable std::shared_mutex mutexFormThreadIdNameMap;

    };
}
