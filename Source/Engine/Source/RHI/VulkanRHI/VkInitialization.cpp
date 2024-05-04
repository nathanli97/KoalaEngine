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
#include "ConsoleVariable.h"
#include "VulkanCommandQueue.h"
#ifdef INCLUDE_RHI_VULKAN
#include "Core.h"
#include "Config.h"
#include "Core/ModuleInterface.h"

#include "VulkanRHI.h"
#include <vulkan/vk_enum_string_helper.h>

static Koala::Logger logger("RHI");
#if RHI_ENABLE_VALIDATION
static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallBack(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *
)
{
    if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        return VK_FALSE;
    std::string messageTypeMessage;
    switch (messageType)
    {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:       messageTypeMessage = "GENERAL";break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:    messageTypeMessage = "VIOLATES_SPECIFICATION";break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:   messageTypeMessage = "PERFORMANCE";break;
    default:break;
    }

    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: logger.debug("Validation: {}: {}", messageTypeMessage, pCallbackData->pMessage);break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:logger.info("Validation: {}: {}", messageTypeMessage, pCallbackData->pMessage);break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:logger.warning("Validation: {}: {}", messageTypeMessage, pCallbackData->pMessage);break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:logger.error("Validation: {}: {}", messageTypeMessage, pCallbackData->pMessage);break;
    default:break;
    }

    return VK_FALSE;
}
static VkDebugUtilsMessengerEXT GVkDebugMessenger{};
#endif

namespace Koala::RHI
{
    static std::vector<const char *> VK_DeviceRequiredExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    bool VulkanRHI::InitVulkanInstance()
    {
        logger.info("Initializating Vulkan(using VK API v{}.{}.{})... ", VK_VERSION_MAJOR(VK_APIVERSION), VK_VERSION_MINOR(VK_APIVERSION), VK_VERSION_PATCH(VK_APIVERSION));
        VkApplicationInfo vkApplicationInfo{};
        VK_CHECK_RESULT_SUCCESS(volkInitialize());

        auto &config = Config::Get();
        auto app_name = config.GetSettingAndWriteDefault("app.name", "Koala Engine", true);
        vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vkApplicationInfo.pApplicationName = app_name.c_str();
        vkApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        vkApplicationInfo.pEngineName = "Koala Engine";
        vkApplicationInfo.engineVersion = VK_MAKE_VERSION(KOALA_ENGINE_VER_MAJOR, KOALA_ENGINE_VER_MINOR, KOALA_ENGINE_VER_PATCH);
        vkApplicationInfo.apiVersion = VK_APIVERSION;

        VkInstanceCreateInfo vkInstanceCreateInfo{};
        vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;

        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char *> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        vkInstanceCreateInfo.enabledLayerCount = 0;

#if RHI_ENABLE_GPU_DEBUG
        const std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };
        vkInstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        vkInstanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

#ifdef __APPLE__
        requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        vkInstanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

        vkInstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        vkInstanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
        VK_CHECK_RESULT_SUCCESS(vkCreateInstance(&vkInstanceCreateInfo, nullptr, &vk.instance));
        volkLoadInstance(vk.instance);
#if RHI_ENABLE_VALIDATION
        VkDebugUtilsMessengerCreateInfoEXT vkDebugUtilsMessengerCreateInfoExt{};
        vkDebugUtilsMessengerCreateInfoExt.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        vkDebugUtilsMessengerCreateInfoExt.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        vkDebugUtilsMessengerCreateInfoExt.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        vkDebugUtilsMessengerCreateInfoExt.pfnUserCallback = VulkanDebugCallBack;
        vkDebugUtilsMessengerCreateInfoExt.pUserData = nullptr;
        VK_CHECK_RESULT_SUCCESS(vkCreateDebugUtilsMessengerEXT(vk.instance,
                                               &vkDebugUtilsMessengerCreateInfoExt,
                                               nullptr,
                                               &GVkDebugMessenger));
#endif

        // Create a window surface for VK WSI

        VK_CHECK_RESULT_SUCCESS(glfwCreateWindowSurface(vk.instance, glfw.window, nullptr, &vk.surfaceKhr))
        return true;
    }
    bool VulkanRHI::ChooseRenderDevice()
    {
        uint32_t countDevices = 0;
        VK_CHECK_RESULT_SUCCESS(vkEnumeratePhysicalDevices(vk.instance, &countDevices, nullptr));
        std::vector<VkPhysicalDevice> physicalDevices(countDevices);
        VK_CHECK_RESULT_SUCCESS(vkEnumeratePhysicalDevices(vk.instance, &countDevices, physicalDevices.data()));

        std::unordered_map<std::string, VkPhysicalDevice> suitableDevices;
        std::unordered_map<std::string, VkPhysicalDeviceProperties> suitableDeviceProps;
        std::unordered_map<std::string, VkPhysicalDeviceFeatures> suitableDevicesFeatures;

        logger.info("We found {} devices which can render Vulkan.", countDevices);

        auto preferedDeviceName = Config::Get().GetSetting("render.device");

        for (const auto device : physicalDevices)
        {
            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            bool bIsDeviceSuitable = true;

            uint32_t deviceExtensionCount = 0;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, nullptr);
            std::vector<VkExtensionProperties> availableExtensions(deviceExtensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, availableExtensions.data());
            for (const std::string &requiredExtensionName : VK_DeviceRequiredExtensions) {
                if (std::find_if(availableExtensions.cbegin(),
                                 availableExtensions.cend(),
                                 [&](const VkExtensionProperties &extensionProperties) -> bool {
                                   return requiredExtensionName == extensionProperties.extensionName;
                                 }) == availableExtensions.cend()) {
                    bIsDeviceSuitable = false;
                    break;
                }
            }

            const std::string vkExtNamePortabilitySubset = "VK_KHR_portability_subset";
            if (std::find_if(availableExtensions.cbegin(), availableExtensions.cend(), [&](const VkExtensionProperties &extensionProperties) -> bool {
                                   return vkExtNamePortabilitySubset == extensionProperties.extensionName;
                             }) != availableExtensions.cend())
            {
                VK_DeviceRequiredExtensions.push_back("VK_KHR_portability_subset");
            }

            if (!bIsDeviceSuitable)
                continue;

            SwapChainSupportDetails chainSupportDetails;

            if (!QuerySwapChainSupport(device, chainSupportDetails)) {
                bIsDeviceSuitable = false;
            }
            if (chainSupportDetails.formats.empty() || chainSupportDetails.presentModes.empty()) {
                bIsDeviceSuitable = false;
            }

            if (!bIsDeviceSuitable)
                continue;

            suitableDevices.emplace(deviceProperties.deviceName, device);
            suitableDeviceProps.emplace(deviceProperties.deviceName, deviceProperties);
            suitableDevicesFeatures.emplace(deviceProperties.deviceName, deviceFeatures);
        }

        if (preferedDeviceName.has_value())
        {
            if (suitableDevices.count(preferedDeviceName.value()) != 0)
            {
                vk.physicalDevice = suitableDevices.at(preferedDeviceName.value());
                logger.info("Choosed prefered GPU: {}", preferedDeviceName.value());
                return true;
            }
            else
            {
                logger.warning("Prefered GPU {} is not avaliable or not suitable", preferedDeviceName.value());
            }
        }


        const bool bNeedPrintGpuInfo = CmdParser::Get().HasArg("printgpus");

        VkPhysicalDevice chooseDevice{nullptr};

        bool bUsingDiscreteGpu = false;
        for (auto const & gpu: suitableDevices)
        {
            auto const &prop = suitableDeviceProps[gpu.first];
            if (bNeedPrintGpuInfo)
                logger.info("GPU '{}' Type={}", gpu.first, string_VkPhysicalDeviceType(prop.deviceType));

            if (!chooseDevice && prop.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                gpu_name = prop.deviceName;
                logger.debug("We choosed GPU '{}' because it is an discreted GPU.", prop.deviceName);
                chooseDevice = gpu.second;
                bUsingDiscreteGpu = true;
            }
        }

        if (!chooseDevice)
        {
            chooseDevice = suitableDevices.begin()->second;
            gpu_name = suitableDevices.begin()->first;
        }

        if (!chooseDevice || suitableDevices.empty())
        {
            logger.error("No suitable GPUs found! Try use --printgpus to check all the suitable GPUs.");
            return false;
        }

        if (!bUsingDiscreteGpu)
        {
            logger.warning("No discrete GPU(s) is found!");
        }

        logger.info("Using GPU: {}", gpu_name);
        vk.physicalDevice = chooseDevice;
        return true;
    }
    bool VulkanRHI::InitVulkanDeviceAndQueue()
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(vk.physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(vk.physicalDevice, &queueFamilyCount, queueFamilies.data());
        
        auto IsCurrentQueueFamilySupportsPresent = [this](uint32_t index)->bool
        {
            VkBool32 bPresentSupported = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(vk.physicalDevice, index, vk.surfaceKhr, &bPresentSupported);
            return bPresentSupported == VK_TRUE;
        };

        std::set<uint32_t> knownQueueFamilies;
        
        auto FindSuitableQueueFamilyIndex = [&, this](bool bAllowQueueFamilyOverlap, VkQueueFlagBits requiredFlag, std::set<uint32_t> notIndexInSet = {}) -> std::optional<uint32_t>
        {
            for (uint32_t index = 0; index < queueFamilyCount; ++index)
            {
                if (notIndexInSet.contains(index))
                    continue;
                auto &queueFamily = queueFamilies[index];
                if (queueFamily.queueFlags & requiredFlag)
                {
                    if (bAllowQueueFamilyOverlap)
                    {
                        return index;
                    }
                    else
                    {
                        if (!knownQueueFamilies.contains(index))
                        {
                            knownQueueFamilies.emplace(index);
                            return index;
                        }
                    }
                }
            }
            return std::optional<uint32_t>();
        };
        

        // We are trying to find queues with different queue families.
        {
            vk.queueInfo.graphicsQueueIndex = FindSuitableQueueFamilyIndex(false, VK_QUEUE_GRAPHICS_BIT);
            vk.queueInfo.transferQueueIndex = FindSuitableQueueFamilyIndex(false, VK_QUEUE_TRANSFER_BIT);
            vk.queueInfo.computeQueueIndex = FindSuitableQueueFamilyIndex(false, VK_QUEUE_COMPUTE_BIT);
            
            if (!vk.queueInfo.graphicsQueueIndex.has_value())
            {
                logger.error("Can not find a queue family that supports Graphics Rendering in your GPU.");
                return false;
            }

            if (!vk.queueInfo.transferQueueIndex.has_value())
            {
                // Fallback, Can we find a compute queue family that not shared with graphics?
                vk.queueInfo.transferQueueIndex = FindSuitableQueueFamilyIndex(true, VK_QUEUE_TRANSFER_BIT, std::set<uint32_t>{vk.queueInfo.graphicsQueueIndex.value()});
                // Fallback, allow overlap with anything queue families.
                if (!vk.queueInfo.transferQueueIndex.has_value())
                    vk.queueInfo.transferQueueIndex = FindSuitableQueueFamilyIndex(true, VK_QUEUE_TRANSFER_BIT);
                
                if (!vk.queueInfo.transferQueueIndex.has_value())
                {
                    logger.error("Can not find a queue family that supports Transfer (Memory Copy) in your GPU.");
                    return false;
                }
            }

            if (!vk.queueInfo.computeQueueIndex.has_value())
            {
                // Can we find a compute queue family that not shared with graphics and transfer?
                vk.queueInfo.computeQueueIndex = FindSuitableQueueFamilyIndex(true, VK_QUEUE_COMPUTE_BIT, std::set<uint32_t>{
                    vk.queueInfo.graphicsQueueIndex.value(),
                    vk.queueInfo.transferQueueIndex.value()
                });
                // Fallback, Can we find a compute queue family that not shared with transfer?
                if (!vk.queueInfo.computeQueueIndex.has_value())
                    vk.queueInfo.computeQueueIndex = FindSuitableQueueFamilyIndex(true, VK_QUEUE_COMPUTE_BIT, std::set<uint32_t>{vk.queueInfo.transferQueueIndex.value()});
                // Fallback, Can we find a compute queue family that not shared with graphics?
                if (!vk.queueInfo.computeQueueIndex.has_value())
                    vk.queueInfo.computeQueueIndex = FindSuitableQueueFamilyIndex(true, VK_QUEUE_COMPUTE_BIT, std::set<uint32_t>{vk.queueInfo.graphicsQueueIndex.value()});
                // Fallback, allow overlap with anything queue families.
                if (!vk.queueInfo.computeQueueIndex.has_value())
                    vk.queueInfo.computeQueueIndex = FindSuitableQueueFamilyIndex(true, VK_QUEUE_COMPUTE_BIT);

                if (!vk.queueInfo.computeQueueIndex.has_value())
                {
                    logger.error("Can not find a queue family that supports Compute in your GPU.");
                    return false;
                }
            }

            for (uint32_t index : knownQueueFamilies)
            {
                if (IsCurrentQueueFamilySupportsPresent(index))
                {
                    vk.queueInfo.presentQueueIndex = index;
                    break;
                }
            }
        }

        
        
        if (!vk.queueInfo.IsComplete())
        {
            logger.error("Your device didn't meet the minimum requirements. One or more queues is missing in your GPU: present, graphics, compute, transfer");
            return false;
        }

        logger.info("Queue details: GraphicsQueue={}, PresentQueue={}, ComputeQueue={}, TransferQueue={}",
            vk.queueInfo.graphicsQueueIndex.value(),
            vk.queueInfo.presentQueueIndex.value(),
            vk.queueInfo.computeQueueIndex.value(),
            vk.queueInfo.transferQueueIndex.value()
        );
        
        float queuePriority = 1;

        const std::set<uint32_t> uniqueQueueFamilies = {
            vk.queueInfo.graphicsQueueIndex.value(),
            vk.queueInfo.presentQueueIndex.value(),
            vk.queueInfo.computeQueueIndex.value(),
            vk.queueInfo.transferQueueIndex.value()
        };

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        for (const uint32_t queue_family : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queue_family;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(uniqueQueueFamilies.size());
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledLayerCount = 0;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(VK_DeviceRequiredExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = VK_DeviceRequiredExtensions.data();

#if RHI_ENABLE_VALIDATION
        const std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#endif

        VK_CHECK_RESULT_SUCCESS(vkCreateDevice(vk.physicalDevice, &deviceCreateInfo, nullptr, &vk.device));
        volkLoadDevice(vk.device);
        
        {
            auto &queueRegister = VulkanCommandQueueRegister::Get();
            queueRegister.AddUninitializedCommandQueue(ECommandQueueType::PresentQueue, ECommandQueueType::GraphicsQueue, ECommandQueueType::CommandQueue, ECommandQueueType::TransferQueue);


            auto preset = queueRegister.GetQueue(ECommandQueueType::PresentQueue);
            auto graphics = queueRegister.GetQueue(ECommandQueueType::GraphicsQueue);
            auto compute = queueRegister.GetQueue(ECommandQueueType::CommandQueue);
            auto transfer = queueRegister.GetQueue(ECommandQueueType::TransferQueue);

            preset->queueFamilyIndex = vk.queueInfo.presentQueueIndex.value();
            graphics->queueFamilyIndex = vk.queueInfo.graphicsQueueIndex.value();
            compute->queueFamilyIndex = vk.queueInfo.computeQueueIndex.value();
            transfer->queueFamilyIndex = vk.queueInfo.transferQueueIndex.value();
            
            vkGetDeviceQueue(vk.device, preset->queueFamilyIndex, 0, &preset->vkQueue);
            vkGetDeviceQueue(vk.device, graphics->queueFamilyIndex, 0, &graphics->vkQueue);
            vkGetDeviceQueue(vk.device, compute->queueFamilyIndex, 0, &compute->vkQueue);
            vkGetDeviceQueue(vk.device, transfer->queueFamilyIndex, 0, &transfer->vkQueue);
        }
        
        return true;
    }

    bool VulkanRHI::InitMemoryAlloctor()
    {
        VmaVulkanFunctions vmaVulkanFunctions{};
        vmaVulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vmaVulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
        
        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.vulkanApiVersion = VK_APIVERSION;
        allocatorCreateInfo.physicalDevice = vk.physicalDevice;
        allocatorCreateInfo.device = vk.device;
        allocatorCreateInfo.instance = vk.instance;
        allocatorCreateInfo.pVulkanFunctions = &vmaVulkanFunctions;

        VK_CHECK_RESULT_SUCCESS(vmaCreateAllocator(&allocatorCreateInfo, &vk.vmaAllocator));

        return true;
    }

    bool VulkanRHI::CreateSwapChain()
    {
        VkExtent2D extent;

        SwapChainSupportDetails chainSupportDetails;
        QuerySwapChainSupport(vk.physicalDevice, chainSupportDetails);

        VkSurfaceFormatKHR surface_format = chainSupportDetails.formats[0];
        for (const auto &format: chainSupportDetails.formats)
        {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                surface_format = format;
            }
        }

        auto enable_vsync = Config::Get().GetSetting("render.vsync", "True").value() == "True";
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

        if (!enable_vsync)
        {
            bool bMailboxSupported = false;
            bool bFifoRelaxedSupported = false;
            bool bImmediateSupported = false;
            for (auto const mode: chainSupportDetails.presentModes)
            {
                if (!bMailboxSupported && mode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    bMailboxSupported = true;
                    continue;
                }
                if (!bFifoRelaxedSupported && mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
                {
                    bFifoRelaxedSupported = true;
                    continue;
                }
                if (!bImmediateSupported && mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                {
                    bImmediateSupported = true;
                    continue;
                }
            }

            if (bMailboxSupported)
            {
                presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            } else if (bFifoRelaxedSupported)
            {
                presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
            } else if (bImmediateSupported)
            {
                presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            }
        }

        logger.debug("Present mode: {}", string_VkPresentModeKHR(presentMode));


        if (chainSupportDetails.capabilities.currentExtent.width != UINT32_MAX)
        {
            extent = chainSupportDetails.capabilities.currentExtent;
        }
        else
        {
            extent = GetFrameBufferSizeFromGLFW(chainSupportDetails.capabilities);
        }

        if (extent.width == 0 || extent.height == 0)
        {
            // window minimized?
            logger.debug("Framebuffer width or height is 0. Maybe window has minimized");
            return false;
        }

        uint32_t imageCount = chainSupportDetails.capabilities.minImageCount + 1;
        if(chainSupportDetails.capabilities.maxImageCount > 0 && imageCount > chainSupportDetails.capabilities.maxImageCount)
        {
            logger.warning("SwapChain supported image count too small -- selecting minimal value of image count");
            imageCount = chainSupportDetails.capabilities.minImageCount;
        }

        VkSwapchainCreateInfoKHR swapchainCreateInfo{ .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainCreateInfo.surface = vk.surfaceKhr;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = surface_format.format;
        swapchainCreateInfo.imageColorSpace = surface_format.colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.imageArrayLayers = 1;

        swapchainCreateInfo.imageUsage =
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT;


        uint32_t swapchainQueueFamilies[] = {
            vk.queueInfo.graphicsQueueIndex.value(),
            vk.queueInfo.presentQueueIndex.value()
        };

        if (vk.queueInfo.graphicsQueueIndex.value() != vk.queueInfo.presentQueueIndex.value())
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = swapchainQueueFamilies;
        }
        else
        {
            // If graphics and present queues are same queue, then we don't need concurrently use two queues.
            // Because we can use same queue for graphics and present.
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;
            swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        swapchainCreateInfo.preTransform = chainSupportDetails.capabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        VK_CHECK_RESULT_SUCCESS(vkCreateSwapchainKHR(vk.device, &swapchainCreateInfo, nullptr, &vk.swapChain.swapchainKhr));
        
        vkGetSwapchainImagesKHR(vk.device, vk.swapChain.swapchainKhr, &imageCount, nullptr);
        vk.swapChain.images.resize(imageCount);
        vkGetSwapchainImagesKHR(vk.device, vk.swapChain.swapchainKhr, &imageCount, vk.swapChain.images.data());
        vk.swapChain.imageFormat = surface_format.format;
        vk.swapChain.imageExtent = extent;
        
        return true;
    }

    bool VulkanRHI::CreateSwapChainViews()
    {
        vk.swapChain.imageViews.resize(vk.swapChain.images.size());

        uint32_t index = 0;
        for (auto &image: vk.swapChain.images)
        {
            VkImageViewCreateInfo vkImageViewCreateInfo{};
            vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            vkImageViewCreateInfo.image = image;
            vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            vkImageViewCreateInfo.format = vk.swapChain.imageFormat;
            // We don't need swizzle for now.
            vkImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            vkImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            vkImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            vkImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            vkImageViewCreateInfo.subresourceRange.levelCount = 1;
            vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            vkImageViewCreateInfo.subresourceRange.layerCount = 1;

            VK_CHECK_RESULT_SUCCESS(vkCreateImageView(vk.device, &vkImageViewCreateInfo, nullptr, &vk.swapChain.imageViews[index++]))
        }
        return true;
    }




    bool VulkanRHI::QuerySwapChainSupport(VkPhysicalDevice device, SwapChainSupportDetails& chainSupportDetails)
    {
        VK_CHECK_RESULT_SUCCESS(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vk.surfaceKhr, &chainSupportDetails.capabilities))
        uint32_t formatCount{};
        VK_CHECK_RESULT_SUCCESS(vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk.surfaceKhr, &formatCount, nullptr))

        if (formatCount != 0) {
            chainSupportDetails.formats.resize(formatCount);
            VK_CHECK_RESULT_SUCCESS(vkGetPhysicalDeviceSurfaceFormatsKHR(device, vk.surfaceKhr, &formatCount, chainSupportDetails.formats.data()));
        }

        uint32_t presentModeCount{};
        VK_CHECK_RESULT_SUCCESS(vkGetPhysicalDeviceSurfacePresentModesKHR(device, vk.surfaceKhr, &presentModeCount, nullptr))
        if (presentModeCount != 0) {
            chainSupportDetails.presentModes.resize(presentModeCount);
            VK_CHECK_RESULT_SUCCESS(vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                                                               vk.surfaceKhr,
                                                               &presentModeCount,
                                                               chainSupportDetails.presentModes.data()))
        }
        
        return true;
    }

    void VulkanRHI::VulkanShutdown()
    {
        CleanSwapChain();

        if (!vk.instance)
        {
            return;
        }

        if (vk.device)
        {
            vkDestroyDevice(vk.device, nullptr);
        }

#if RHI_ENABLE_VALIDATION
        if (GVkDebugMessenger)
        {
            vkDestroyDebugUtilsMessengerEXT(vk.instance, GVkDebugMessenger, nullptr);
        }
#endif

        if (vk.surfaceKhr)
        {
            vkDestroySurfaceKHR(vk.instance, vk.surfaceKhr, nullptr);
        }
        vkDestroyInstance(vk.instance, nullptr);
    }

}
#endif