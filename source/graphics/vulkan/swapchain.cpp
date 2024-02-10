//
// Created by Sam on 2024-02-07.
//

#include "graphics/vulkan/swapchain.h"
#include "io/logger.h"
#include "util/require.h"

using namespace dn;
using namespace dn::vulkan;

SwapchainSupportDetails vulkan::querySwapchainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
    SwapchainSupportDetails details;

    details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
    details.formats = device.getSurfaceFormatsKHR(surface);
    details.presentModes = device.getSurfacePresentModesKHR(surface);

    return details;
}

vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    vk::SurfaceFormatKHR out = availableFormats[0];

    for (const auto &availableFormat: availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            out = availableFormat;
        }
    }
    log::v("Picked Swapchain Surface Format:", "\n\tFormat:", to_string(out.format), "\n\tColor Space:", to_string(out.colorSpace));

    return out;
}

vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes, bool requestUncapped) {
    const std::array<vk::PresentModeKHR, 3> presentModePreferences{vk::PresentModeKHR::eFifo, vk::PresentModeKHR::eImmediate, vk::PresentModeKHR::eMailbox};
    const std::array<std::string, 3> presentModeNames{"V-Sync", "Uncapped", "Triple-Buffering"};
    uint32_t currentIndex = 0;

    if (requestUncapped) {
        for (const auto &availablePresentMode: availablePresentModes) {
            for (uint32_t i = 0; i < presentModePreferences.size(); ++i) {
                if (availablePresentMode == presentModePreferences[i] && i > currentIndex) {
                    currentIndex = i;
                }
            }
        }
    }

    log::i("Picked Present Mode:", presentModeNames[currentIndex]);
    return presentModePreferences[currentIndex];
}

vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, Window &window) {
    vk::Extent2D out;
    // If the current extents are set to the maximum values,
    // the window is trying to tell us to set it manually.
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        out = capabilities.currentExtent;
    } else {
        int w, h;
        glfwGetFramebufferSize(window.mGlfwWindow, &w, &h);
        out = vk::Extent2D{static_cast<uint32_t>(w), static_cast<uint32_t>(h)};

        out.width = std::clamp(out.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        out.height = std::clamp(out.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }

    log::i("Frame extents set to:", out.width, "*", out.height);
    return out;
}

Swapchain::Swapchain(
        Window &window,
        vk::PhysicalDevice physicalDevice,
        vk::Device device,
        vk::SurfaceKHR surface,
        const QueueFamilyIndices &queueFamilyIndices,
        SwapchainConfiguration config
) : mConfig(config), mDevice(device) {
    log::d("Creating Swapchain");

    SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice, surface);

    mSurfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
    mPresentMode = chooseSwapPresentMode(swapchainSupport.presentModes, mConfig.uncappedFramerate);
    mExtent = chooseSwapExtent(swapchainSupport.capabilities, window);

    if (mExtent.width < 1 || mExtent.height < 1) {
        log::v("Invalid swapchain extents. Retry later!");
        mNeedsNewSwapchain = true;
        return;
    }

    // One more image than the minimum to avoid stalling if the driver is still working on the image
    uint32_t minImageCount = swapchainSupport.capabilities.minImageCount + 1;
    if (swapchainSupport.capabilities.maxImageCount > 0 &&
        minImageCount > swapchainSupport.capabilities.maxImageCount) {
        minImageCount = swapchainSupport.capabilities.maxImageCount;
    }
    log::d("Creating the swapchain with at least", minImageCount, "images");

    const std::array<uint32_t, 2> queueIndices{queueFamilyIndices.graphicsFamily.value(), queueFamilyIndices.presentFamily.value()};
    vk::SharingMode imageSharingMode = vk::SharingMode::eExclusive;
    uint32_t queueFamilyIndexCount = 0;
    if (!queueFamilyIndices.isUnifiedGraphicsPresentQueue()) {
        imageSharingMode = vk::SharingMode::eConcurrent;
        queueFamilyIndexCount = 2;
    }

    // TODO switch to VK_IMAGE_USAGE_TRANSFER_DST_BIT for post processing, instead of directly rendering to the SC
    vk::SwapchainCreateInfoKHR createInfo{
            {},
            surface,
            minImageCount,
            mSurfaceFormat.format,
            mSurfaceFormat.colorSpace,
            mExtent,
            1, // Can be 2 for 3D, etc.
            {vk::ImageUsageFlagBits::eColorAttachment},
            imageSharingMode,
            queueFamilyIndexCount,
            queueIndices.data(),
            swapchainSupport.capabilities.currentTransform, // Do not add any swapchain transforms beyond the default
            vk::CompositeAlphaFlagBitsKHR::eOpaque, // Do not blend with other windows
            mPresentMode,
            vk::True, // Clip pixels if obscured by other window -> Perf+
            nullptr // TODO Put previous swapchain here if overridden, e.g. if window size changed
    };

    mSwapchain = device.createSwapchainKHR(createInfo);

    std::vector<vk::Image> images = device.getSwapchainImagesKHR(mSwapchain);
    for (const vk::Image image: images) {
        mImages.emplace_back(mDevice, image, nullptr);
        mImageViews.emplace_back(device, mImages.back(), ImageViewConfiguration{mSurfaceFormat.format});
    }

    vk::Format depthFormat = findDepthFormat(physicalDevice);
    mDepthImage.emplace(
            mDevice, physicalDevice,
            ImageConfiguration{
                    mExtent.width,
                    mExtent.height,
                    depthFormat,
                    vk::ImageTiling::eOptimal,
                    vk::ImageUsageFlagBits::eDepthStencilAttachment,
                    vk::MemoryPropertyFlagBits::eDeviceLocal
            }
    );
    mDepthImageView.emplace(
            mDevice,
            mDepthImage.value(),
            ImageViewConfiguration{depthFormat, vk::ImageAspectFlagBits::eDepth}
    );

    // TODO the following
//    RenderPasses::create();
//    createFramebuffers();

    mNeedsNewSwapchain = false;
}

uint32_t Swapchain::getWidth() const {
    return mExtent.width;
}

uint32_t Swapchain::getHeight() const {
    return mExtent.height;
}

float Swapchain::getAspectRatio() const {
    return static_cast<float>(mExtent.width) / static_cast<float>(mExtent.height);
}

Swapchain::~Swapchain() {
    log::d("Destroying Swapchain");

    // TODO destroy all
//    for (auto &swapchainFramebuffer: Swapchain::framebuffers) {
//        vkDestroyFramebuffer(Devices::logical, swapchainFramebuffer, nullptr);
//    }
//
//    RenderPasses::destroy();

    mDepthImageView.reset();
    mDepthImage.reset();

    mImageViews.clear();

    mDevice.destroy(mSwapchain);
}
