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
#include <unordered_map>

#include "Core/ModuleInterface.h"
#include "Core/ThreadInterface.h"

namespace Koala::FileIO
{
    enum class EDiskSpeedGrade
    {
        SuperFast = 5,     // NVME SSD or above?
        Fast      = 4,     // SATA SSD?
        Normal    = 3,     // High Speed HDD?
        Slow      = 2,     // Normal HDD?
        SuperSlow = 1      // Slower than normal HDD (Maybe network mounted disk)
    };

    // Setting for uplimit of Read/Write threads
    const inline std::unordered_map<EDiskSpeedGrade, std::pair<int, int>> IOThreadNumDiskGradeMapping{
        {EDiskSpeedGrade::SuperFast, {16, 2}},
        {EDiskSpeedGrade::Fast, {8, 2}},
        {EDiskSpeedGrade::Normal, {4, 1}},
        {EDiskSpeedGrade::Slow, {2, 1}},
        {EDiskSpeedGrade::SuperSlow, {1, 1}},
    };
    
    class FileIOManager: IModule
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(FileIOManager)
        bool Initialize_MainThread() override;
        bool Shutdown_MainThread() override;
        void Tick_MainThread(float deltaTime) override;

    private:
        uint32_t numReadThreads{1};
        uint32_t numWriteThreads{1};

        std::vector<IThread*> writeThreadHandles;
        std::vector<IThread*> readThreadHandles;

        EDiskSpeedGrade diskSpeedGrade{EDiskSpeedGrade::Normal};
    };
}
