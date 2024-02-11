//
// Created by Sam on 2024-02-07.
//

#ifndef DOUGHNUTSANDBOX_SWAPCHAIN_H
#define DOUGHNUTSANDBOX_SWAPCHAIN_H

#include "io/window.h"
#include "graphics/vulkan/queue_family_indices.h"
#include "image.h"
#include "image_view.h"
#include "framebuffer.h"
#include "instance.h"
#include "render_pass.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct SwapchainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities{};
        std::vector<vk::SurfaceFormatKHR> formats{};
        std::vector<vk::PresentModeKHR> presentModes{};
    };

    SwapchainSupportDetails querySwapchainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);

    struct SwapchainConfiguration {
        bool uncappedFramerate;
    };

    // TODO Recreate Swapchain when needed
    class Swapchain {
    public:
        Swapchain(
                Instance &instance,
                SwapchainConfiguration config
        );

        Swapchain(const Swapchain &other) = delete;

        Swapchain(Swapchain &&other) = delete;

        ~Swapchain();

        bool shouldRecreateSwapchain();

        bool recreateSwapchain();

        [[nodiscard]] uint32_t getWidth() const;

        [[nodiscard]] uint32_t getHeight() const;

        [[nodiscard]] float getAspectRatio() const;

    private:
        SwapchainConfiguration mConfig;
        Instance &mInstance;

        bool mNeedsNewSwapchain;
        vk::Extent2D mExtent{};
        vk::SurfaceFormatKHR mSurfaceFormat{};
        vk::PresentModeKHR mPresentMode{};
        vk::SwapchainKHR mSwapchain = nullptr;
        std::vector<Image> mImages{};
        std::vector<ImageView> mImageViews{};
        vk::Format mDepthFormat{};
        std::optional<Image> mDepthImage{};
        std::optional<ImageView> mDepthImageView{};
        std::optional<RenderPass> mRenderPass{};
        std::vector<Framebuffer> mFramebuffers{};
    };
}

#endif //DOUGHNUTSANDBOX_SWAPCHAIN_H
