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

#include "Core/Check.h"
#include "Core/SingletonInterface.h"
#include "Core/ThreadInterface.h"
#include "ThreadNames.h"

namespace Koala
{
    class ThreadManager final: public ISingleton
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(ThreadManager)
        void CreateThread(IThread* inThread, EThreadName inThreadName = EThreadName::UnknownThread)
        {
            std::lock_guard lock(mutexForThreadList);
            threads.emplace_back(
                [=, this]
                {
                    RegisterThreadWithName(inThreadName);
                    inThread->Run();
                });
        }
        template <typename Callable, typename... Args>
        void CreateThread(Callable inFunc, Args&&... args) requires std::invocable<Callable, Args...>
        {
            std::lock_guard lock(mutexForThreadList);
            threads.emplace_back(
                [=, this]
                {
                    inFunc(std::forward<Args>(args)...);
                });
        }
        // Register current thread by given name.
        // WARNING: This logic should happen in early-engine init stage. All threads should be created in this stage.
        void RegisterThreadWithName(EThreadName inThread)
        {
            std::unique_lock lock(mutexFormThreadIdNameMap);
            auto id = std::this_thread::get_id();
            if (mapThreadIdToName.contains(id))
                mapThreadIdToName[id] = inThread;
            else
                mapThreadIdToName.emplace(id, inThread);
        }
        // Get Current Thread Name.
        // WARNING: Only call after engine early-initialized.
        EThreadName GetCurrentThread() const
        {
            auto id = std::this_thread::get_id();
            if (!mapThreadIdToName.contains(id))
                return EThreadName::UnknownThread;
            return mapThreadIdToName.at(id);
        }
        // Get Current Thread Name. This is thread-safe version of GetCurrentThread(), but slower.
        // Call this function only in early engine init stage.
        EThreadName GetCurrentThreadInEarlyEngineInitStage() const
        {
            std::shared_lock lock(mutexFormThreadIdNameMap);
            auto id = std::this_thread::get_id();
            if (!mapThreadIdToName.contains(id))
                return EThreadName::UnknownThread;
            return mapThreadIdToName.at(id);
        }
        ~ThreadManager()
        {
            for (auto & thread: threads)
            {
                if (thread.joinable())
                    thread.join();
            }
            threads.clear();
        }
    private:
        std::list<std::thread> threads;
        mutable std::mutex mutexForThreadList;
        std::unordered_map<std::thread::id, EThreadName> mapThreadIdToName;
        mutable std::shared_mutex mutexFormThreadIdNameMap;

    };
}
