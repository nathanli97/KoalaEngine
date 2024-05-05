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

#include "Worker/Worker.h"

namespace Koala
{
    void Worker::Run()
    {
        status.store(EWorkerStatus::Idle, std::memory_order::release);
        
        while(true)
        {
            if (bShouldExit.load(std::memory_order::acquire))
            {
                if (status.load(std::memory_order::acquire) != EWorkerStatus::Ready)
                    return;
            }
            
            {
                std::unique_lock lock(mutex);

                while (status != EWorkerStatus::Ready)
                    cvWorkerWaitTask.wait(lock);
            }

            // We are status == Ready now.
            status.store(EWorkerStatus::Busy, std::memory_order::seq_cst);

            task(taskArg);

            // Task Finished!

            status.store(EWorkerStatus::Finished, std::memory_order::release);

            {
                std::scoped_lock lock(mutex);
                cvWorkerTaskFinish.notify_all();
            }

            if (bShouldExit.load(std::memory_order::acquire))
            {
                return;
            }
        }
    }
}
