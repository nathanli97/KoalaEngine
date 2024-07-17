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

#include "Core/ThreadInterface.h"

#include <functional>

#include "FileIOTask.h"

namespace Koala::FileIO
{
    class FileIOThread: public IThread
    {
    public:
        FileIOThread() = default;
        void Run() override;
        bool IsIdle() const
        {
            return atomicTaskInProgress.load() == false;
        }
        void ShutdownIOThread();
        virtual void DoIOTask() = 0;
    protected:
        std::atomic<bool> atomicHasPendingTask{false};
        std::atomic<bool> atomicShouldShutdown{false};
        std::atomic<bool> atomicTaskInProgress{false};
    };

    class FileReadIOThread final: public FileIOThread
    {
    public:
        void SetTask(FileReadIOTask &&);
        void DoIOTask() override;

    private:
        FileReadIOTask task{};
    };
}
