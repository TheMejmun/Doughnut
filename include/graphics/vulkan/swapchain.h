//
// Created by Sam on 2024-02-07.
//

#ifndef DOUGHNUT_SWAPCHAIN_H
#define DOUGHNUT_SWAPCHAIN_H

#include "io/window.h"
#include "graphics/vulkan/queue_family_indices.h"
#include "graphics/vulkan/handles/image.h"
#include "graphics/vulkan/handles/image_view.h"
#include "graphics/vulkan/handles/framebuffer.h"
#include "context.h"
#include "graphics/vulkan/handles/render_pass.h"
#include "core/late_init.h"
#include "graphics/vulkan/handles/semaphore.h"

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
                Context &context,
                SwapchainConfiguration config
        );

        Swapchain(const Swapchain &other) = delete;

        Swapchain(Swapchain &&other) = delete;

        ~Swapchain();

        [[nodiscard]] bool shouldRecreate() const;

        void recreate();

        [[nodiscard]] float getAspectRatio() const;

        /**
         * @param semaphore The semaphore to wait on
         * @return True, if an image was acquired
         */
        std::optional<uint32_t> acquireNextImage(Semaphore &semaphore);

        Framebuffer& getFramebuffer(uint32_t i);

        LateInit<RenderPass> mRenderPass{};

        vk::SwapchainKHR mSwapchain = nullptr;

        uint32_t mMinImageCount;
        uint32_t mImageCount;
        vk::Extent2D mExtent{};

    private:
        void create();

        void destroy();

        SwapchainConfiguration mConfig;
        Context &mContext;

        bool mNeedsNewSwapchain;
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
