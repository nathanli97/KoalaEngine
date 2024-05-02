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

#include <algorithm>
#if INCLUDE_RHI_VULKAN
#include <string>
#include <spdlog/spdlog.h>

#include "Engine.h"
#include "Config.h"
#include "Core/Module.h"
#include "VulkanRHI.h"

namespace Koala::RHI
{
    static void GLFW_OnFrameBufferResized(GLFWwindow *window, int w, int h)
    {
        auto rhi = static_cast<VulkanRHI*>(glfwGetWindowUserPointer(window));
    }

    bool VulkanRHI::GLFWInitialize()
    {
        auto &config = Config::Get();

        int window_width = std::stoi(config.GetSettingStrWithAutoSaving("render.window.width", "1280", true));
        int window_height = std::stoi(config.GetSettingStrWithAutoSaving("render.window.height", "960", true));

        std::string window_title = config.GetSettingStrWithAutoSaving("render.window.title", "Koala");

        int ret = glfwInit();
        if (ret != GLFW_TRUE)
        {
            spdlog::error("GLFW Initialization error");
            return false;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfw.window = glfwCreateWindow(window_width,window_height,window_title.c_str(),nullptr,nullptr);

        if (!glfw.window)
        {
            spdlog::error("Failed to create GLFW Window");
            return false;
        }

        glfwSetWindowUserPointer(glfw.window, this);
        glfwSetFramebufferSizeCallback(glfw.window, GLFW_OnFrameBufferResized);

        return true;
    }

    void VulkanRHI::GLFWShutdown()
    {
        if (glfw.window)
        {
            glfwDestroyWindow(glfw.window);
        }

        glfwTerminate();
    }

    bool VulkanRHI::GLFWTick()
    {
        if (glfwWindowShouldClose(glfw.window))
        {
            Engine::Get().RequestEngineStop();
            return false;
        }

        glfwPollEvents();

        return true;
    }

    VkExtent2D VulkanRHI::GetFrameBufferSizeFromGLFW(const VkSurfaceCapabilitiesKHR &capabilities)
    {
        int w,h;
        glfwGetFramebufferSize(glfw.window, &w, &h);

        VkExtent2D extent;
        extent.width = std::clamp((uint32_t) w, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height =
            std::clamp((uint32_t) h, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return extent;
    }


}
#endif