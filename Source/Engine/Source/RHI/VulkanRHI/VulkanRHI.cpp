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

    void VulkanRHI::Shutdown_RenderThread()
    {
        VulkanShutdown();
    }

    bool VulkanRHI::Tick_MainThread()
    {
        return GLFWTick();
    }

    void VulkanRHI::CleanSwapChain()
    {
        for (auto &view: vk.swapChain.imageViews)
        {
            if (view)
            {
                vkDestroyImageView(renderDevice->device, view, nullptr);
            }
        }

        if (vk.swapChain.swapchainKhr)
        {
            vkDestroySwapchainKHR(renderDevice->device, vk.swapChain.swapchainKhr, nullptr);
        }
        vk.swapChain.images.clear();
    }

    void VulkanRHI::GetRenderDevices(std::forward_list<const IRenderDevice *>& outDevices)
    {
        for (const auto &device: vulkanRenderDevices)
        {
            outDevices.push_front(&device);
        }
    }

    const IRenderDevice * VulkanRHI::GetUsingRenderDevice()
    {
        return renderDevice;
    }

    ITextureInterface* VulkanRHI::GetTextureInterface()
    {
        return &VulkanTextureInterface::Get();
    }

    IBufferInterface *VulkanRHI::GetBufferInterface()
    {
        return &VulkanBufferInterface::Get();
    }


    void VulkanRHI::HandleVulkanFuncFailed(VkResult result, const char* func, const char* file, size_t line)
    {
        auto message = string_VkResult(result);
        logger.error("The call '{}' failed with error {} in file {}, line {}", func, message, file, line);
        std::abort();
    }

}
#endif