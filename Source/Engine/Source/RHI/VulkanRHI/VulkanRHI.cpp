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

#include "Core/KoalaLogger.h"
#ifdef INCLUDE_RHI_VULKAN
#include "VulkanRHI.h"
#include <vulkan/vk_enum_string_helper.h>
#include "VulkanBufferRHI.h"
#include "VulkanTextureRHI.h"


namespace Koala::RHI {
    static Logger logger("VulkanRHI");
    bool VulkanRHI::PreInit_MainThread()
    {
        return GLFWInitialize();
    }

    void VulkanRHI::PostShutdown_MainThread()
    {
        GLFWShutdown();
    }

    bool VulkanRHI::Initialize_RenderThread()
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

    void VulkanRHI::Shutdown_RenderThread()
    {
        VulkanShutdown();
    }

    bool VulkanRHI::Tick_MainThread()
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

    ITextureInterface* VulkanRHI::GetTextureInterface()
    {
        static VulkanTextureInterface vkTextureInterface(vk);
        return &vkTextureInterface;
    }

    IBufferInterface *VulkanRHI::GetBufferInterface()
    {
        static VulkanBufferInterface vkBufferInterface(vk);
        return &vkBufferInterface;
    }


    void VulkanRHI::HandleVulkanFuncFailed(VkResult result, const char* func, const char* file, size_t line)
    {
        auto message = string_VkResult(result);
        logger.error("The call '{}' failed with error {} in file {}, line {}", func, message, file, line);
        std::abort();
    }

}
#endif