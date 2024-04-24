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
#include <vk_mem_alloc.h>

#ifdef INCLUDE_RHI_VULKAN
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#define VK_APIVERSION VK_API_VERSION_1_3


namespace Koala::RHI
{
    struct VulkanRuntime {
        VkInstance instance{};
        VkSurfaceKHR surface_khr{};
        VkPhysicalDevice physical_device{};

        VmaAllocator vma_allocator{};

        struct
        {
            std::optional<uint32_t> graphics_queue_index;
            std::optional<uint32_t> compute_queue_index;
            std::optional<uint32_t> present_queue_index;

            [[nodiscard]] bool IsComplete() const
            {
                return graphics_queue_index.has_value() &&
                    compute_queue_index.has_value() &&
                    present_queue_index.has_value();
            }
        } queue_info;

        VkQueue present_queue{};
        VkQueue compute_queue{};
        VkQueue graphics_queue{};

        VkDevice device{};

        struct
        {
            std::vector<VkImage> images;
            std::vector<VkImageView> image_views;
            VkFormat image_format{};
            VkExtent2D image_extent{};
            VkSwapchainKHR swapchain_khr{};
        } swap_chain;
    };
    struct GLFWRuntime {
        GLFWwindow *window{};
    };
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };
}
#endif
