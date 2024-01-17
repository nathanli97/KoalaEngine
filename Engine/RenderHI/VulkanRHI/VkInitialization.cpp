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

#include "CmdParser.h"
#ifdef INCLUDE_RHI_VULKAN
#include <spdlog/spdlog.h>

#include "Config.h"
#include "EngineVersion.h"
#include "Module.h"
#include "VulkanRHI.h"
#include <vulkan/vk_enum_string_helper.h>
#if RHI_ENABLE_VALIDATION
static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *
)
{
    std::string message_type;
    switch (messageType)
    {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:message_type = "GENERAL";break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:message_type = "VIOLATES_SPECIFICATION";break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:message_type = "PERFORMANCE";break;
    default:break;
    }

    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: spdlog::debug("RHI Validation: {}: {}", message_type, pCallbackData->pMessage);break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:spdlog::info("RHI Validation: {}: {}", message_type, pCallbackData->pMessage);break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:spdlog::warn("RHI Validation: {}: {}", message_type, pCallbackData->pMessage);break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:spdlog::error("RHI Validation: {}: {}", message_type, pCallbackData->pMessage);break;
    default:break;
    }

    return VK_FALSE;
}
VkResult VulkanCreateDebugUtilsMessengerEXT(VkInstance instance,
                                            const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                            const VkAllocationCallbacks *pAllocator,
                                            VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
static VkDebugUtilsMessengerEXT vk_debug_messenger{};
#endif

namespace Koala::RenderHI
{
    const static std::vector<const char *> VK_DeviceRequiredExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
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
        vk_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
        vk_create_info.ppEnabledLayerNames = validation_layers.data();
        required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

#ifdef __APPLE__
        required_extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        vk_create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

        vk_create_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
        vk_create_info.ppEnabledExtensionNames = required_extensions.data();
        VkResult result = vkCreateInstance(&vk_create_info, nullptr, &vk.instance);

        if (result != VK_SUCCESS)
        {
            spdlog::error("RHI: Failed to create VK Instance: {}", string_VkResult(result));
            return false;
        }
#if RHI_ENABLE_VALIDATION
        VkDebugUtilsMessengerCreateInfoEXT vk_debug_utils_messenger_create_info_ext{};
        vk_debug_utils_messenger_create_info_ext.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        vk_debug_utils_messenger_create_info_ext.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        vk_debug_utils_messenger_create_info_ext.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        vk_debug_utils_messenger_create_info_ext.pfnUserCallback = VulkanDebugCallBack;
        vk_debug_utils_messenger_create_info_ext.pUserData = nullptr;
        if (VulkanCreateDebugUtilsMessengerEXT(vk.instance,
                                               &vk_debug_utils_messenger_create_info_ext,
                                               nullptr,
                                               &vk_debug_messenger) != VK_SUCCESS) {
            spdlog::error("RHI: Failed to create vulkan validation messenger");
        }
        else
        {
            spdlog::info("RHI: Vulkan validation layer is enabled and initialized successfully");
        }
#endif

        // Create a window surface for VK WSI

        result = glfwCreateWindowSurface(vk.instance, glfw.window, nullptr, &vk.surface_khr);
        if (result != VK_SUCCESS)
        {
            spdlog::error("RHI: Failed to create window surface for vulkan: {}", string_VkResult(result));
            return false;
        }
        return true;
    }
    bool VulkanRHI::ChooseRenderDevice()
    {
        uint32_t count_devices = 0;
        auto result = vkEnumeratePhysicalDevices(vk.instance, &count_devices, nullptr);
        std::vector<VkPhysicalDevice> physical_devices(count_devices);
        result = vkEnumeratePhysicalDevices(vk.instance, &count_devices, physical_devices.data());

        std::unordered_map<std::string, VkPhysicalDevice> suitable_devices;
        std::unordered_map<std::string, VkPhysicalDeviceProperties> suitable_device_props;
        std::unordered_map<std::string, VkPhysicalDeviceFeatures> suitable_devices_features;


        if (result != VK_SUCCESS) {
            spdlog::error("RHI: Failed to enumerate VK physical devices: {}", string_VkResult(result));
            return false;
        }

        spdlog::info("RHI: We found {} devices which can render Vulkan.", count_devices);

        auto prefered_device_name = IModule::Get<Config>().GetSettingStr("render.device");

        for (const auto device : physical_devices)
        {
            VkPhysicalDeviceProperties device_properties;
            VkPhysicalDeviceFeatures device_features;
            vkGetPhysicalDeviceProperties(device, &device_properties);
            vkGetPhysicalDeviceFeatures(device, &device_features);

            bool is_this_device_suitable = true;

            uint32_t device_extension_count = 0;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &device_extension_count, nullptr);
            std::vector<VkExtensionProperties> available_extensions(device_extension_count);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &device_extension_count, available_extensions.data());
            for (const std::string &required_extension_name : VK_DeviceRequiredExtensions) {
                if (std::find_if(available_extensions.cbegin(),
                                 available_extensions.cend(),
                                 [&](const VkExtensionProperties &extension_properties) -> bool {
                                   return required_extension_name == extension_properties.extensionName;
                                 }) == available_extensions.cend()) {
                    is_this_device_suitable = false;
                    break;
                }
            }

            if (!is_this_device_suitable)
                continue;

            SwapChainSupportDetails chain_support_details;

            if (!QuerySwapChainSupport(device, chain_support_details)) {
                is_this_device_suitable = false;
            }
            if (chain_support_details.formats.empty() || chain_support_details.present_modes.empty()) {
                is_this_device_suitable = false;
            }

            if (!is_this_device_suitable)
                continue;

            suitable_devices.emplace(device_properties.deviceName, device);
            suitable_device_props.emplace(device_properties.deviceName, device_properties);
            suitable_devices_features.emplace(device_properties.deviceName, device_features);
        }

        if (prefered_device_name.has_value())
        {
            if (suitable_devices.count(prefered_device_name.value()) != 0)
            {
                vk.physical_device = suitable_devices.at(prefered_device_name.value());
                spdlog::info("RHI: Choosed prefered GPU: {}", prefered_device_name.value());
                return true;
            }
            else
            {
                spdlog::warn("RHI: Prefered GPU {} is not avaliable or not suitable", prefered_device_name.value());
            }
        }


        const bool print_gpu_info = CmdParser::Get().HasArg("printgpus");

        VkPhysicalDevice choose_device{nullptr};

        for (auto const & gpu: suitable_devices)
        {
            auto const &prop = suitable_device_props[gpu.first];
            if (print_gpu_info)
                spdlog::info("RHI: GPU '{}' Type={}", gpu.first, string_VkPhysicalDeviceType(prop.deviceType));

            if (!choose_device && prop.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                gpu_name = prop.deviceName;
                spdlog::debug("RHI: We choosed GPU '{}' because it is an discreted GPU.", prop.deviceName);
                choose_device = gpu.second;
            }
        }

        if (!choose_device)
        {
            choose_device = suitable_devices.begin()->second;
            gpu_name = suitable_devices.begin()->first;
        }

        if (!choose_device || suitable_devices.empty())
        {
            spdlog::error("RHI: No suitable GPUs found! Try use --printgpus to check all the suitable GPUs.");
            return false;
        }

        vk.physical_device = choose_device;
        return true;
    }
    bool VulkanRHI::InitVulkanQueue()
    {
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vk.physical_device, &queue_family_count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(vk.physical_device, &queue_family_count, queue_families.data());

        uint32_t index = 0;
        for (auto const &queue_family: queue_families)
        {
            if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT && queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                vk.queue_info.graphics_queue_index = vk.queue_info.compute_queue_index = index;
            } else if (!vk.queue_info.graphics_queue_index.has_value() && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                vk.queue_info.graphics_queue_index = index;
            } else if (!vk.queue_info.compute_queue_index.has_value() && queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                vk.queue_info.compute_queue_index = index;
            }

            if (!vk.queue_info.present_queue_index.has_value())
            {
                VkBool32 is_present_supported = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(vk.physical_device, index, vk.surface_khr, &is_present_supported);
                if (is_present_supported)
                {
                    vk.queue_info.present_queue_index = index;
                }
            }
            index++;

            // All the queue families we need is found, no further check is needed.
            if (vk.queue_info.IsComplete())
                break;
        }

        if (vk.queue_info.IsComplete())
        {
            spdlog::info("RHI: Device passed all checks");
        } else if (vk.queue_info.IsMinimumSupported())
        {
            spdlog::warn("RHI: Your device didn't supports all features we need. the rendering is limited.");
        } else
        {
            spdlog::error("RHI: Your device didn't meet the minimum requirements.");
            return false;
        }

        return true;
    }


    bool VulkanRHI::QuerySwapChainSupport(VkPhysicalDevice device, SwapChainSupportDetails& chain_support_details)
    {
        VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vk.surface_khr, &chain_support_details.capabilities);
        if (result != VK_SUCCESS) {
            return false;
        }
        uint32_t format_count{};
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk.surface_khr, &format_count, nullptr);

        if (format_count != 0) {
            chain_support_details.formats.resize(format_count);
            result =
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk.surface_khr, &format_count, chain_support_details.formats.data());
        }

        if (result != VK_SUCCESS) {
            spdlog::error("RHI: Failed to query VK physical device surface format {}", string_VkResult(result));
            return false;
        }

        uint32_t present_mode_count{};
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk.surface_khr, &present_mode_count, nullptr);
        if (present_mode_count != 0) {
            chain_support_details.present_modes.resize(present_mode_count);
            result = vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                                                               vk.surface_khr,
                                                               &present_mode_count,
                                                               chain_support_details.present_modes.data());
        }

        if (result != VK_SUCCESS) {
            spdlog::warn("RHI: Failed to query VK physical device surface present modes: {}", string_VkResult(result));
            return false;
        }

        return true;
    }

}
#endif