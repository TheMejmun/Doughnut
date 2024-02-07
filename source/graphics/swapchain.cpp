//
// Created by Sam on 2024-02-07.
//

#include "graphics/swapchain.h"
#include "io/logger.h"
#include "util/require.h"

using namespace dn;

SwapchainSupportDetails dn::querySwapchainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
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
        glfwGetFramebufferSize(window.glfwWindow, &w, &h);
        out = vk::Extent2D{static_cast<uint32_t>(w), static_cast<uint32_t>(h)};

        out.width = std::clamp(out.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        out.height = std::clamp(out.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }

    log::i("Frame extents set to:", out.width, "*", out.height);
    return out;
}

VulkanSwapchain::VulkanSwapchain(
        Window &window,
        vk::PhysicalDevice physicalDevice,
        vk::Device device,
        vk::SurfaceKHR surface,
        const QueueFamilyIndices &queueFamilyIndices,
        bool requestUncapped
) : mDevice(device) {
    log::d("Creating VulkanSwapchain");

    SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice, surface);

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
    vk::PresentModeKHR presentModeTemp = chooseSwapPresentMode(swapchainSupport.presentModes, requestUncapped);
    vk::Extent2D extentTemp = chooseSwapExtent(swapchainSupport.capabilities, window);

    if (extentTemp.width < 1 || extentTemp.height < 1) {
        log::v("Invalid swapchain extents. Retry later!");
        mNeedsNewSwapchain = true;
        // TODO return false;
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
    vk::SwapchainCreateInfoKHR createInfo(
            {},
            surface,
            minImageCount,
            surfaceFormat.format,
            surfaceFormat.colorSpace,
            extentTemp,
            1, // Can be 2 for 3D, etc.
            {vk::ImageUsageFlagBits::eColorAttachment},
            imageSharingMode,
            queueFamilyIndexCount,
            queueIndices.data(),
            swapchainSupport.capabilities.currentTransform, // Do not add any swapchain transforms beyond the default
            vk::CompositeAlphaFlagBitsKHR::eOpaque, // Do not blend with other windows
            presentModeTemp,
            vk::True, // Clip pixels if obscured by other window -> Perf+
            nullptr // TODO Put previous swapchain here if overridden, e.g. if window size changed
    );

    mSwapchain = device.createSwapchainKHR(createInfo);
    require(mSwapchain != nullptr, "Failed to create swapchain!");

    // imageCount only specified a minimum!
    // TODO the following
//    vkGetSwapchainImagesKHR(Devices::logical, Swapchain::swapchain, &Swapchain::imageCount, nullptr);
//    Swapchain::images.resize(Swapchain::imageCount);
//    vkGetSwapchainImagesKHR(Devices::logical, Swapchain::swapchain, &Swapchain::imageCount,
//                            Swapchain::images.data());
//    Swapchain::imageFormat = surfaceFormat.format;
//    Swapchain::extent = extentTemp;
//    Swapchain::presentMode = presentModeTemp;
//
//    Swapchain::createImageViews();
//    Swapchain::createDepthResources();
//    RenderPasses::create();
//    createFramebuffers();

    // TODO return true;
}

uint32_t VulkanSwapchain::getWidth() const {
    return mExtent.width;
}

uint32_t VulkanSwapchain::getHeight() const {
    return mExtent.height;
}

float VulkanSwapchain::getAspectRatio() const {
    return static_cast<float>(mExtent.width) / static_cast<float>(mExtent.height);
}

VulkanSwapchain::~VulkanSwapchain() {
    log::d("Destroying VulkanSwapchain");

    // TODO destroy all
//    for (auto &swapchainFramebuffer: Swapchain::framebuffers) {
//        vkDestroyFramebuffer(Devices::logical, swapchainFramebuffer, nullptr);
//    }
//
//    RenderPasses::destroy();
//
//    vkDestroyImageView(Devices::logical, Swapchain::depthImageView, nullptr);
//    vkDestroyImage(Devices::logical, Swapchain::depthImage, nullptr);
//    vkFreeMemory(Devices::logical, Swapchain::depthImageMemory, nullptr);
//
//    for (auto &swapchainImageView: Swapchain::imageViews) {
//        vkDestroyImageView(Devices::logical, swapchainImageView, nullptr);
//    }

    mDevice.destroy(mSwapchain);
}
