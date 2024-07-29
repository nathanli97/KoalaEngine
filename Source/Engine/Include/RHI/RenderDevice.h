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
#include <forward_list>
#include <string>

namespace Koala::RHI
{
    enum class ERenderDeviceType
    {
        Unknown,
        Integrated,
        Dedicated
    };
    class IRenderDevice
    {
    public:
        std::string name;
        ERenderDeviceType type{ERenderDeviceType::Unknown};
        bool bSuitable;

#ifdef RHI_ENABLE_GPU_DEBUG
        std::forward_list<std::string> nonSuitableReasonsForDebug;
#endif
        const void* GetPlatformNativePointer() const {return this;}
        void* GetPlatformNativePointer() {return this;}
    };
}
