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
#include "VulkanRenderDevice.h"

#ifdef INCLUDE_RHI_VULKAN
#include "Runtime.h"
#include "RHI/RHI.h"

namespace Koala::RHI {

class VulkanRHI: public IRenderHardware{
public:
    KOALA_IMPLEMENT_SINGLETON(VulkanRHI)
    // ===== Caller: RT =======
    bool Initialize_RenderThread() override;
    void Shutdown_RenderThread() override;
    void GetRenderDevices(std::forward_list<const IRenderDevice *>&) override;
    const IRenderDevice * GetUsingRenderDevice() override;
    ITextureInterface* GetTextureInterface() override;
    IBufferInterface * GetBufferInterface() override;

    // ===== Caller: MainThread =======
    bool PreInit_MainThread() override;
    void PostShutdown_MainThread() override;
    bool Tick_MainThread() override;

    NODISCARD FORCEINLINE static VulkanRuntime* GetVkRuntime()
    {
        return &Get().vk;
    }

    NODISCARD FORCEINLINE static VulkanRenderDevice* GetVkRenderDevice()
    {
        return Get().renderDevice;
    }
    
    static void HandleVulkanFuncFailed(VkResult result, const char * func, const char * file, size_t line);
private:
    VulkanRuntime vk{};
    GLFWRuntime glfw{};

    std::forward_list<VulkanRenderDevice> vulkanRenderDevices;
    VulkanRenderDevice*                   renderDevice{};

    // std::string gpu_name;

    bool GLFWInitialize();
    void GLFWShutdown();

    // Ticking GLFW.
    // Return true: continue to rendering
    // Return false: stop rendering, the window is closing.
    bool GLFWTick();

    // Initialize Vulkan
    bool VulkanInitialize();
    void VulkanShutdown();

    // ------Vulkan Initialization Functions------
    bool InitializeVulkanInstance();
    void DiscoverRenderDevices(std::forward_list<VulkanRenderDevice> &outRenderDevices);
    bool ChooseRenderDevice();
    bool InitVulkanDeviceAndQueue();
    bool InitMemoryAlloctor();
    bool CreateSwapChain();
    bool CreateSwapChainViews();

    void QuerySwapChainSupport(VkPhysicalDevice device, VulkanSwapChainSupportDetails &chainSupportDetails);

    VkExtent2D GetFrameBufferSizeFromGLFW(const VkSurfaceCapabilitiesKHR &capabilities);


    void CleanSwapChain();


};

}
#endif