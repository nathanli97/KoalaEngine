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

#ifdef INCLUDE_RHI_VULKAN
#include "VulkanRHI.h"


namespace Koala::RenderHI {
    bool VulkanRHI::PreInit()
    {
        return GLFWInitialize();
    }

    void VulkanRHI::PostShutdown()
    {
        GLFWShutdown();
    }

    bool VulkanRHI::Initialize()
    {
        return
            VulkanInitialize();
    }

    const char* VulkanRHI::GetGPUName()
    {
        if (gpu_name.empty())
            return nullptr;
        return gpu_name.c_str();
    }

    void VulkanRHI::Shutdown()
    {
        VulkanShutdown();
    }

    bool VulkanRHI::Tick()
    {
        return GLFWTick();
    }

    bool VulkanRHI::VulkanInitialize()
    {
        return InitVulkanInstance() &&
            ChooseRenderDevice() &&
            InitVulkanDeviceAndQueue() &&
            CreateSwapChain() &&
            CreateSwapChainViews();
    }


    void VulkanRHI::CleanSwapChain()
    {
        for (auto &view: vk.swap_chain.image_views)
        {
            if (view)
            {
                vkDestroyImageView(vk.device, view, nullptr);
            }
        }

        if (vk.swap_chain.swapchain_khr)
        {
            vkDestroySwapchainKHR(vk.device, vk.swap_chain.swapchain_khr, nullptr);
        }
        vk.swap_chain.images.clear();
    }


}
#endif