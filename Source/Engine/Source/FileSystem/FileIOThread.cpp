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

#include "FileSystem/FileIOThread.h"

namespace Koala::FileIO
{
    void FileIOThread::Run()
    {
        while (!atomicShouldShutdown.load())
        {
            while (!task)
                atomicHasNewTask.wait(true);
            
            if (atomicShouldShutdown.load() == true)
                break;

            task();

            atomicTaskInProgress.store(false);
            atomicTaskInProgress.notify_all();
        }
    }

    void FileIOThread::AssignNewTask(std::function<void()> newTask)
    {
        task = newTask;
        atomicTaskInProgress.store(false);
        atomicHasNewTask.store(true);
        atomicHasNewTask.notify_all();
    }

    void FileIOThread::ShutdownIOThread()
    {
        while (atomicTaskInProgress.load() == true)
            atomicTaskInProgress.wait(false);
        // assign a new dummy task
        task = []() {};
        atomicShouldShutdown.store(true);
        atomicShouldShutdown.notify_all();
    }
}
