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

    struct SwapchainConfiguration{
        bool uncappedFramerate;
    };

    class VulkanSwapchain {
    public:
        VulkanSwapchain(
                Window &window,
                vk::PhysicalDevice physicalDevice,
                vk::Device device,
                vk::SurfaceKHR surface,
                const QueueFamilyIndices &queueFamilyIndices,
                SwapchainConfiguration config
        );

        ~VulkanSwapchain();

        bool shouldRecreateSwapchain();

        bool recreateSwapchain();

        [[nodiscard]] uint32_t getWidth() const;

        [[nodiscard]] uint32_t getHeight() const;

        [[nodiscard]] float getAspectRatio() const;

    private:
        SwapchainConfiguration mConfig;

        vk::Device mDevice= nullptr;

        bool mNeedsNewSwapchain;
        vk::Extent2D mExtent{};
        vk::SurfaceFormatKHR mSurfaceFormat{};
        vk::PresentModeKHR mPresentMode{};
        vk::SwapchainKHR mSwapchain = nullptr;
        std::vector<vk::Image> mImages{};
    };
}

#endif //DOUGHNUTSANDBOX_SWAPCHAIN_H
