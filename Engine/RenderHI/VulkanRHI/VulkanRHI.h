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

#ifdef INCLUDE_RHI_VULKAN
#include "Runtime.h"
#include "RenderHI/RenderHI.h"

namespace Koala::RenderHI {

class VulkanRHI: public RenderHI{
public:
    // ===== Caller: RT =======
    bool Initialize() override;
    void Shutdown() override;
    const char* GetGPUName() override;


    // ===== Caller: MainThread =======
    bool PreInit() override;
    void PostShutdown() override;
    bool Tick() override;
private:
    VulkanRuntime vk;
    GLFWRuntime glfw;

    std::string gpu_name;

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
    bool InitVulkanInstance();
    bool ChooseRenderDevice();
    bool InitVulkanQueue();
    bool CreateSwapChain();

    bool QuerySwapChainSupport(VkPhysicalDevice device, SwapChainSupportDetails &chain_support_details);

    VkExtent2D GetFrameBufferSizeFromGLFW(const VkSurfaceCapabilitiesKHR &capabilities);



};

}
#endif