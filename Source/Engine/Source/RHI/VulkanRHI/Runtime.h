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
#include <optional>
#include <vector>
#define VMA_VULKAN_VERSION 1003000
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <set>
#include <volk.h>
#include <vk_mem_alloc.h>

#include "Memory/Allocator.h"

#ifdef INCLUDE_RHI_VULKAN
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#define VK_APIVERSION VK_API_VERSION_1_3
#define VK_CHECK_RESULT_SUCCESS(Func) { VkResult result = Func; if (result != VK_SUCCESS){ VulkanRHI::HandleVulkanFuncFailed(result, #Func, __FILE__, __LINE__); }}
// #define VK_CHECK_RESULT_NOT_LESS_THEN_SUCCESS(Func) { VkResult result = Func; if (result < VK_SUCCESS){ VulkanRHI::HandleVulkanFuncFailed(result, #Func, __FILE__, __LINE__); }}


namespace Koala::RHI
{
    // Deprecated: This struct will be removed in the future.
    // Do not add new members in this struct!
    struct VulkanRuntime {
        VkInstance instance{};
        VkSurfaceKHR surfaceKhr{};

        VmaAllocator vmaAllocator{};

        struct
        {
            std::optional<uint32_t> graphicsQueueIndex;
            std::optional<uint32_t> computeQueueIndex;
            std::optional<uint32_t> presentQueueIndex;
            std::optional<uint32_t> transferQueueIndex;
            
            NODISCARD FORCEINLINE bool IsComplete() const
            {
                return graphicsQueueIndex.has_value() &&
                    computeQueueIndex.has_value() &&
                    presentQueueIndex.has_value() &&
                    transferQueueIndex.has_value();
            }
        } queueInfo;
        
        struct
        {
            std::vector<VkImage> images;
            std::vector<VkImageView> imageViews;
            VkFormat imageFormat{};
            VkExtent2D imageExtent{};
            VkSwapchainKHR swapchainKhr{};
        } swapChain;
    };
    struct GLFWRuntime {
        GLFWwindow *window{};
    };
    struct VulkanSwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
}
#endif
