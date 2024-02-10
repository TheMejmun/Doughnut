//
// Created by Sam on 2024-02-07.
//

#ifndef DOUGHNUTSANDBOX_SWAPCHAIN_H
#define DOUGHNUTSANDBOX_SWAPCHAIN_H

#include "io/window.h"
#include "graphics/vulkan/queue_family_indices.h"
#include "image.h"
#include "image_view.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct SwapchainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities{};
        std::vector<vk::SurfaceFormatKHR> formats{};
        std::vector<vk::PresentModeKHR> presentModes{};
    };

    SwapchainSupportDetails querySwapchainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);

    struct SwapchainConfiguration{
        bool uncappedFramerate;
    };

    class Swapchain {
    public:
        Swapchain(
                Window &window,
                vk::PhysicalDevice physicalDevice,
                vk::Device device,
                vk::SurfaceKHR surface,
                const QueueFamilyIndices &queueFamilyIndices,
                SwapchainConfiguration config
        );

        ~Swapchain();

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
        std::vector<Image> mImages{};
        std::vector<ImageView> mImageViews{};
        std::optional<Image> mDepthImage;
        std::optional<ImageView> mDepthImageView;
    };
}

#endif //DOUGHNUTSANDBOX_SWAPCHAIN_H
