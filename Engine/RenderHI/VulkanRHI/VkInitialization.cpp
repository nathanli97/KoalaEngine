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

#include <spdlog/spdlog.h>

#include "Config.h"
#include "EngineVersion.h"
#include "Module.h"
#include "VulkanRHI.h"
#include <vulkan/vk_enum_string_helper.h>

namespace Koala::RenderHI
{
    bool VulkanRHI::InitVulkanInstance()
    {
        VkApplicationInfo app_info{};

        auto &config = IModule::Get<Config>();
        auto app_name = config.GetSettingStrWithAutoSaving("app.name", "Koala Engine", true);
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = app_name.c_str();
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "Koala Engine";
        app_info.engineVersion = VK_MAKE_VERSION(KOALA_ENGINE_VER_MAJOR, KOALA_ENGINE_VER_MINOR, KOALA_ENGINE_VER_PATCH);
        app_info.apiVersion = VK_API_VERSION_1_2;

        VkInstanceCreateInfo vk_create_info{};
        vk_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        vk_create_info.pApplicationInfo = &app_info;

        uint32_t glfw_extension_count = 0;
        const char **glfw_extensions;

        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
        std::vector<const char *> required_extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

        uint32_t extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

        vk_create_info.enabledLayerCount = 0;

#if RHI_ENABLE_VALIDATION
        const std::vector<const char *> validation_layers = {
            "VK_LAYER_KHRONOS_validation"
        };
        create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        create_info.ppEnabledLayerNames = validation_layers.data();
        required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

#ifdef __APPLE__
        required_extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

        vk_create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
        vk_create_info.ppEnabledExtensionNames = required_extensions.data();
        VkResult result = vkCreateInstance(&vk_create_info, nullptr, &vk.instance);

        if (result != VK_SUCCESS)
        {
            spdlog::error("RHI: Failed to create VK Instance: {}", string_VkResult(result));
            return false;
        }

        return true;
    }

}