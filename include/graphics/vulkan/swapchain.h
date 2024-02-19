//
// Created by Sam on 2024-02-07.
//

#ifndef DOUGHNUT_SWAPCHAIN_H
#define DOUGHNUT_SWAPCHAIN_H

#include "io/window.h"
#include "graphics/vulkan/queue_family_indices.h"
#include "image.h"
#include "image_view.h"
#include "framebuffer.h"
#include "instance.h"
#include "render_pass.h"
#include "core/late_init.h"
#include "semaphore.h"

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
    // TODO Frames in Flight
    class Swapchain {
    public:
        Swapchain(
                Instance &instance,
                SwapchainConfiguration config
        );

        Swapchain(const Swapchain &other) = delete;

        Swapchain(Swapchain &&other) = delete;

        ~Swapchain();

        [[nodiscard]] bool shouldRecreate() const;

        void recreate();

        [[nodiscard]] uint32_t getWidth() const;

        [[nodiscard]] uint32_t getHeight() const;

        [[nodiscard]] float getAspectRatio() const;

        /**
         * @param semaphore The semaphore to wait on
         * @return True, if an image was acquired
         */
        std::optional<uint32_t> acquireNextImage(Semaphore &semaphore);

        Framebuffer& getFramebuffer(uint32_t i);

        LateInit<RenderPass> mRenderPass{};

        uint32_t getImageCount();

        vk::SwapchainKHR mSwapchain = nullptr;

    private:
        void create();

        void destroy();

        SwapchainConfiguration mConfig;
        Instance &mInstance;

        bool mNeedsNewSwapchain;
        vk::Extent2D mExtent{};
        vk::SurfaceFormatKHR mSurfaceFormat{};
        vk::PresentModeKHR mPresentMode{};
        std::vector<Image> mImages{};
        std::vector<ImageView> mImageViews{};
        LateInit<Image> mDepthImage{};
        LateInit<ImageView> mDepthImageView{};
        std::vector<Framebuffer> mFramebuffers{};
    };
}

#endif //DOUGHNUT_SWAPCHAIN_H
