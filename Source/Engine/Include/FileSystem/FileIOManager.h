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
#include <stdbool.h>
#include <unordered_map>
#include <unordered_set>

#include "Core/StringHash.h"
#include "File.h"
#include "FileIOTask.h"
#include "Core/ModuleInterface.h"
#include "Core/ThreadInterface.h"

namespace Koala::FileIO
{
    class FileIOManager: IModule
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(FileIOManager)
        bool Initialize_MainThread() override;
        bool Shutdown_MainThread() override;
        void Tick_MainThread(float deltaTime) override;

        void RequestReadFileAsync(FileHandle inHandle, size_t offset, size_t size, void *buffer, FileIOCallback callback = nullptr);
        void RequestWriteFileAsync(FileHandle inHandle, size_t offset, size_t size, void *buffer, FileIOCallback callback = nullptr);
    private:
        void TickFileReadIOThread(size_t threadIdx);

        uint32_t numReadThreads{4};
        uint32_t numWriteThreads{2};

        std::vector<IThread*> writeThreadHandles;
        std::vector<IThread*> readThreadHandles;

        std::unordered_map<FileHandle, uint32_t> readingFileMap_ThreadIdx;
        std::unordered_set<FileHandle, uint32_t> writingFileSet_ThreadIdx;
        
        // std::queue<FileReadIOTask> remainingReadTasks;
        // std::queue<FileWriteIOTask> remainingWriteTasks;
    };
}