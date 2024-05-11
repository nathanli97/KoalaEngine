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
    static std::vector<const char *> VulkanDeviceRequiredExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    bool VulkanRHI::VulkanInitialize()
    {
        if (!InitializeVulkanInstance())
            return false;
        
        DiscoverRenderDevices(vulkanRenderDevices);
        
        if (!ChooseRenderDevice())
            return false;
        
        if (!InitVulkanDeviceAndQueue())
            return false;
        
        if (!InitMemoryAlloctor())
            return false;
        
        if (!CreateSwapChain())
            return false;
        
        if (!CreateSwapChainViews())
            return false;

        return true;
    }
    
    bool VulkanRHI::InitializeVulkanInstance()
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
    
    void VulkanRHI::DiscoverRenderDevices(std::forward_list<VulkanRenderDevice> &outRenderDevices)
    {
        outRenderDevices.clear();
        
        uint32_t countDevices = 0;
        VK_CHECK_RESULT_SUCCESS(vkEnumeratePhysicalDevices(vk.instance, &countDevices, nullptr))
        std::vector<VkPhysicalDevice> physicalDevices(countDevices);
        VK_CHECK_RESULT_SUCCESS(vkEnumeratePhysicalDevices(vk.instance, &countDevices, physicalDevices.data()))
        
        for (const auto device : physicalDevices)
        {
            VulkanRenderDevice vulkanRenderDevice;
            vulkanRenderDevice.bSuitable = true;

            vulkanRenderDevice.physicalDevice = device;
            vkGetPhysicalDeviceProperties(device, &vulkanRenderDevice.physicalDeviceProperties);
            vkGetPhysicalDeviceFeatures(device, &vulkanRenderDevice.physicalDeviceFeatures);

            uint32_t deviceExtensionCount = 0;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, nullptr);
            vulkanRenderDevice.physicalDeviceExtensions.resize(deviceExtensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, vulkanRenderDevice.physicalDeviceExtensions.data());

            for (const std::string &requiredExtensionName : VulkanDeviceRequiredExtensions) {
                if (std::find_if(vulkanRenderDevice.physicalDeviceExtensions.cbegin(),
                                 vulkanRenderDevice.physicalDeviceExtensions.cend(),
                                 [&](const VkExtensionProperties &extensionProperties) -> bool {
                                   return requiredExtensionName == extensionProperties.extensionName;
                                 }) == vulkanRenderDevice.physicalDeviceExtensions.cend()) {
                    vulkanRenderDevice.bSuitable = false;
#ifdef RHI_ENABLE_GPU_DEBUG
                    vulkanRenderDevice.nonSuitableReasonsForDebug.push_front(fmt::format("Missing required Vulkan device extension: {}", requiredExtensionName));
#endif
                    break;
                }
            }

            
            VulkanSwapChainSupportDetails chainSupportDetails;

            QuerySwapChainSupport(device, chainSupportDetails);
            
            if (chainSupportDetails.formats.empty() || chainSupportDetails.presentModes.empty()) {
                vulkanRenderDevice.bSuitable = false;
#ifdef RHI_ENABLE_GPU_DEBUG
                vulkanRenderDevice.nonSuitableReasonsForDebug.push_front(fmt::format("Can not find suitable swap chain support on this device"));
#endif
            }
            vulkanRenderDevice.name = vulkanRenderDevice.physicalDeviceProperties.deviceName;
            vulkanRenderDevice.type = vulkanRenderDevice.physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? ERenderDeviceType::Dedicated :ERenderDeviceType::Integrated;
            outRenderDevices.push_front(vulkanRenderDevice);
        }
    }

    bool VulkanRHI::ChooseRenderDevice()
    {
        auto preferedDeviceName = Config::Get().GetSetting("render.device");

        std::unordered_map<std::string, VulkanRenderDevice*> devices;

        for (VulkanRenderDevice &device: vulkanRenderDevices)
        {
            devices.emplace(device.name, &device);
        }

        if (preferedDeviceName.has_value())
        {
            if (devices.contains(preferedDeviceName.value()))
            {
                renderDevice = devices.at(preferedDeviceName.value());
                logger.info("Choosed prefered GPU: {}", preferedDeviceName.value());
                return true;
            }
            else
            {
                logger.warning("Prefered GPU {} is not avaliable or not suitable", preferedDeviceName.value());
            }
        }
        
        for (auto const & device: devices)
        {
            auto const &prop = device.second->physicalDeviceProperties;

            if (!renderDevice && device.second->bSuitable && device.second->type == ERenderDeviceType::Dedicated)
            {
                logger.debug("We choose GPU '{}' because it is an dedicated GPU.", prop.deviceName);
                renderDevice = device.second;
            }
        }

        if (!renderDevice)
        {
            for (auto const & device: devices)
            {
                auto const &prop = device.second->physicalDeviceProperties;

                if (!renderDevice && device.second->bSuitable)
                {
                    renderDevice = device.second;
                }
            }
        }

        if (!renderDevice)
        {
            logger.error("No suitable GPUs found!");
            return false;
        }

        logger.info("Using GPU: {}, Is Dedicated GPU={}", renderDevice->name, renderDevice->type==ERenderDeviceType::Dedicated);
        return true;
    }
    bool VulkanRHI::InitVulkanDeviceAndQueue()
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(renderDevice->physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(renderDevice->physicalDevice, &queueFamilyCount, queueFamilies.data());
        
        auto IsCurrentQueueFamilySupportsPresent = [this](uint32_t index)->bool
        {
            VkBool32 bPresentSupported = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(renderDevice->physicalDevice, index, vk.surfaceKhr, &bPresentSupported);
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
        
        const std::string vkExtNamePortabilitySubset = "VK_KHR_portability_subset";
        if (std::find_if(renderDevice->physicalDeviceExtensions.cbegin(), renderDevice->physicalDeviceExtensions.cend(), [&](const VkExtensionProperties &extensionProperties) -> bool {
                               return vkExtNamePortabilitySubset == extensionProperties.extensionName;
                         }) != renderDevice->physicalDeviceExtensions.cend())
        {
            VulkanDeviceRequiredExtensions.push_back("VK_KHR_portability_subset");
        }
        
        VkPhysicalDeviceFeatures deviceFeatures{};
        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(uniqueQueueFamilies.size());
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledLayerCount = 0;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(VulkanDeviceRequiredExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = VulkanDeviceRequiredExtensions.data();

#if RHI_ENABLE_VALIDATION
        const std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#endif

        VK_CHECK_RESULT_SUCCESS(vkCreateDevice(renderDevice->physicalDevice, &deviceCreateInfo, nullptr, &renderDevice->device));
        volkLoadDevice(renderDevice->device);
        
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
            
            vkGetDeviceQueue(renderDevice->device, preset->queueFamilyIndex, 0, &preset->vkQueue);
            vkGetDeviceQueue(renderDevice->device, graphics->queueFamilyIndex, 0, &graphics->vkQueue);
            vkGetDeviceQueue(renderDevice->device, compute->queueFamilyIndex, 0, &compute->vkQueue);
            vkGetDeviceQueue(renderDevice->device, transfer->queueFamilyIndex, 0, &transfer->vkQueue);
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
        allocatorCreateInfo.physicalDevice = renderDevice->physicalDevice;
        allocatorCreateInfo.device = renderDevice->device;
        allocatorCreateInfo.instance = vk.instance;
        allocatorCreateInfo.pVulkanFunctions = &vmaVulkanFunctions;

        VK_CHECK_RESULT_SUCCESS(vmaCreateAllocator(&allocatorCreateInfo, &vk.vmaAllocator));

        return true;
    }

    bool VulkanRHI::CreateSwapChain()
    {
        VkExtent2D extent;

        VulkanSwapChainSupportDetails chainSupportDetails;
        QuerySwapChainSupport(renderDevice->physicalDevice, chainSupportDetails);

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

        VK_CHECK_RESULT_SUCCESS(vkCreateSwapchainKHR(renderDevice->device, &swapchainCreateInfo, nullptr, &vk.swapChain.swapchainKhr));
        
        vkGetSwapchainImagesKHR(renderDevice->device, vk.swapChain.swapchainKhr, &imageCount, nullptr);
        vk.swapChain.images.resize(imageCount);
        vkGetSwapchainImagesKHR(renderDevice->device, vk.swapChain.swapchainKhr, &imageCount, vk.swapChain.images.data());
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

            VK_CHECK_RESULT_SUCCESS(vkCreateImageView(renderDevice->device, &vkImageViewCreateInfo, nullptr, &vk.swapChain.imageViews[index++]))
        }
        return true;
    }




    void VulkanRHI::QuerySwapChainSupport(VkPhysicalDevice device, VulkanSwapChainSupportDetails& chainSupportDetails)
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
    }

    void VulkanRHI::VulkanShutdown()
    {
        CleanSwapChain();

        if (!vk.instance)
        {
            return;
        }

        if (renderDevice && renderDevice->device)
        {
            vkDestroyDevice(renderDevice->device, nullptr);
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