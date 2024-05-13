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

#include "FileSystem/IOTask.h"

#include "Core/KoalaLogger.h"

namespace Koala::FileIO
{
    Logger logger("FileIO");
    void IOTask::DoIoWork(void *inPtr)
    {
        auto thisPtr = static_cast<IOTask*>(inPtr);

        size_t ioSize = thisPtr->size;
        std::shared_ptr fileHandle = thisPtr->fileHandle;

        if (!fileHandle->IsVaild() || thisPtr->RequiredShouldCancel())
        {
            logger.debug("Canceled the file {} IO Request because this file already closed", fileHandle->fileName.GetString());
            return;
        }

        if (thisPtr->offset >= fileHandle->fileSize)
        {
            logger.error("Requested load offset is bigger than file size: {}", fileHandle->fileName.GetString());
            return;
        }
        
        if (ioSize > fileHandle->fileSize - thisPtr->offset)
        {
            logger.warning("Requested load area (offset + size) is out of readable area of this file: {}", fileHandle->fileName.GetString());
            ioSize = fileHandle->fileSize - thisPtr->offset;
        }

        if (fileHandle->currOffset != thisPtr->offset)
        {
            int64_t delta = thisPtr->offset - fileHandle->currOffset;
            fileHandle->fileStream.seekg(delta, std::ios::cur);
        }

        auto currOffset = thisPtr->offset;
        auto endOffset = currOffset + ioSize;
        
        if (thisPtr->type == EIOTaskType::IORead)
        {
            if (!(fileHandle->openmode & std::ios::in))
            {
                logger.error("Can not read the file {} because this file is not opened for read!", fileHandle->fileName.GetString());
                return;
            }
            fileHandle->fileStream.read((char*)thisPtr->buffer, (int64_t)ioSize);
        } else if (thisPtr->type == EIOTaskType::IOWrite)
        {
            if (!(fileHandle->openmode & std::ios::out))
            {
                logger.error("Can not write the file {} because this file is not opened for write!", fileHandle->fileName.GetString());
                return;
            }
            fileHandle->fileStream.write((char*)thisPtr->buffer, (int64_t)ioSize);
        }
    }
}
