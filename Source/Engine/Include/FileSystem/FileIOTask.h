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
#include <queue>

#include "File.h"

namespace Koala::FileIO
{
    typedef std::function<void(bool bOk, int64_t size, const void *buffer)> FileIOCallback;
    struct FileIOTask
    {
        int64_t offset{0};
        int64_t remainingSize{0};
        int64_t performedSize{0};
        void   *bufferStart;
        FileIOCallback callback;
        FileHandle handle;

        // Indicates status is good (no error)
        uint8_t  bOK             :  1{true};
        // Indicates task has been finished (success or error or canceled)
        uint8_t  bFinished       :  1{false};
        uint8_t  bCanceled       :  1{false};
        uint8_t  bCompleted      :  1{false};
        uint8_t  bCancelRequested:  1{false};
    };
}
