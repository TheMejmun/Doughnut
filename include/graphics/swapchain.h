//
// Created by Sam on 2024-02-07.
//

#ifndef DOUGHNUTSANDBOX_SWAPCHAIN_H
#define DOUGHNUTSANDBOX_SWAPCHAIN_H

#include "io/window.h"
#include "graphics/queue_family_indices.h"

#include <vulkan/vulkan.hpp>

namespace dn {
    struct SwapchainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities{};
        std::vector<vk::SurfaceFormatKHR> formats{};
        std::vector<vk::PresentModeKHR> presentModes{};
    };

    SwapchainSupportDetails querySwapchainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);

    class VulkanSwapchain {
    public:
        VulkanSwapchain(
                Window &window,
                vk::PhysicalDevice physicalDevice,
                vk::Device device,
                vk::SurfaceKHR surface,
                const QueueFamilyIndices &queueFamilyIndices,
                bool requestUncapped
        );

        ~VulkanSwapchain();

        bool shouldRecreateSwapchain();

        bool recreateSwapchain();

        [[nodiscard]] uint32_t getWidth() const;

        [[nodiscard]] uint32_t getHeight() const;

        [[nodiscard]] float getAspectRatio() const;

    private:
        vk::Device mDevice= nullptr;

        bool mNeedsNewSwapchain;
        vk::Extent2D mExtent{};
        vk::SwapchainKHR mSwapchain = nullptr;
    };
}

#endif //DOUGHNUTSANDBOX_SWAPCHAIN_H
