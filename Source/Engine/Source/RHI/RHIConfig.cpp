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

#include <unordered_set>

#include "ConsoleVariable.h"
#include "RHI/RHI.h"

#ifdef INCLUDE_RHI_VULKAN
#include "VulkanRHI/VulkanRHI.h"
#endif

namespace Koala::RHI
{
    std::unordered_set<std::string> GetAvaliableRHIs()
    {
        return std::unordered_set<std::string>{
#ifdef INCLUDE_RHI_VULKAN
            "vulkan",
#endif
        };
    }
    IRenderHardware* GetRHI(const std::string& name)
    {
        if (name == "null")
            return nullptr;
#ifdef INCLUDE_RHI_VULKAN
        else if (name == "vulkan")
            return &ISingleton::Get<VulkanRHI>();
#endif
        else return nullptr;
    }
}
