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
#include "File.h"
#include "AsyncWorker/Task.h"

namespace Koala::FileIO
{
    enum class EIOTaskType
    {
        IORead,
        IOWrite
    };

    struct IOTask: AsyncWorker::Task
    {
        static void DoIoWork(void* inPtr);
        IOTask(): Task(DoIoWork, this, EThreadType::UnknownThread, ETaskPriority::Normal){}
        IOTask(FileHandle inFileHanle, void *inBuffer, EIOTaskType inIOTaskType):
            Task(DoIoWork, this, EThreadType::UnknownThread, ETaskPriority::Normal),
            fileHandle(std::move(inFileHanle)),
            buffer(inBuffer),
            type(inIOTaskType) {}
        FileHandle fileHandle;
        size_t      offset, size;
        void        *buffer{nullptr};
        EIOTaskType type{EIOTaskType::IORead};
    };

    typedef std::shared_ptr<IOTask> IOTaskPtr;
}
