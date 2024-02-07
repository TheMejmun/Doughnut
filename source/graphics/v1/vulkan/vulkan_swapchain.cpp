//
// Created by Saman on 24.08.23.
//

#include "graphics/v1/vulkan/vulkan_swapchain.h"
#include "graphics/v1/vulkan/vulkan_instance.h"
#include "graphics/v1/vulkan/vulkan_devices.h"
#include "graphics/v1/vulkan/vulkan_renderpasses.h"
#include "graphics/v1/vulkan/vulkan_memory.h"
#include "graphics/v1/vulkan/vulkan_imgui.h"
#include "graphics/v1/vulkan/vulkan_images.h"
#include "io/logger.h"

#include "GLFW/glfw3.h"
#include <array>
#include <algorithm>

using namespace dn;
using namespace dn::vulkan;

// Global
VkSurfaceKHR Swapchain::surface = nullptr;
uint32_t Swapchain::framebufferWidth = 0, Swapchain::framebufferHeight = 0;
float Swapchain::aspectRatio = 1.0f;
VkSwapchainKHR Swapchain::swapchain = nullptr;
VkFormat Swapchain::imageFormat{};
VkPresentModeKHR Swapchain::presentMode;
VkExtent2D Swapchain::extent{};
std::vector<VkImage> Swapchain::images{};
std::vector<VkImageView> Swapchain::imageViews{};
std::vector<VkFramebuffer> Swapchain::framebuffers{};
bool Swapchain::needsNewSwapchain = false;
uint32_t Swapchain::minImageCount = 2;
uint32_t Swapchain::imageCount = 2;

VkImage Swapchain::depthImage = nullptr;
VkDeviceMemory Swapchain::depthImageMemory = nullptr;
VkImageView Swapchain::depthImageView = nullptr;

// Local
GLFWwindow *window = nullptr;

Swapchain::SwapchainSupportDetails Swapchain::querySwapchainSupport(VkPhysicalDevice device) {
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, Swapchain::surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, Swapchain::surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, Swapchain::surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, Swapchain::surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, Swapchain::surface, &presentModeCount,
                                                  details.presentModes.data());
    }

    return details;
}

void Swapchain::createSurface(GLFWwindow *w) {
    window = w;

    if (glfwCreateWindowSurface(Instance::instance, window, nullptr, &Swapchain::surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
}

VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    VkSurfaceFormatKHR out = availableFormats[0];

    for (const auto &availableFormat: availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            out = availableFormat;
        }
    }

   log::v("Picked Swapchain Surface Format: ");
   log::v("\tFormat:", out.format);
   log::v("\tColor Space:", out.colorSpace);

    return out;
}

VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
    const std::vector<VkPresentModeKHR> presentModePreferences = {
            VK_PRESENT_MODE_FIFO_KHR,
            VK_PRESENT_MODE_IMMEDIATE_KHR,
            VK_PRESENT_MODE_MAILBOX_KHR
    };
    const std::vector<std::string> presentModeNames = {
            "V-Sync",
            "Uncapped",
            "Triple-Buffering"
    };
    uint32_t currentIndex = 0;

    for (const auto &availablePresentMode: availablePresentModes) {
        for (uint32_t i = 0; i < presentModePreferences.size(); ++i) {
            if (availablePresentMode == presentModePreferences[i] && i > currentIndex) {
                currentIndex = i;
            }
        }
    }

   log::i("Picked Swapchain Present Mode:", presentModeNames[currentIndex]);
    return presentModePreferences[currentIndex];
}

VkExtent2D Swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    VkExtent2D out;
    // If the current extents are set to the maximum values,
    // the window manager is trying to tell us to set it manually.
    // Otherwise, return the current value.
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        out = capabilities.currentExtent;
    } else {
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);

        out = {
                static_cast<uint32_t>(w),
                static_cast<uint32_t>(h)
        };

        out.width = std::clamp(out.width,
                               capabilities.minImageExtent.width,
                               capabilities.maxImageExtent.width);
        out.height = std::clamp(out.height,
                                capabilities.minImageExtent.height,
                                capabilities.maxImageExtent.height);
    }

    Swapchain::framebufferWidth = out.width;
    Swapchain::framebufferHeight = out.height;
    Swapchain::aspectRatio = static_cast<float>(out.width) / static_cast<float>(out.height);

   log::i("Swapchain extents set to:", out.width, "*", out.height);
    return out;
}

bool Swapchain::recreateSwapchain(RenderState &state) {
   log::v("Recreating Swapchain");

    // May need to recreate render pass here if e.g. window moves to HDR monitor

    vkDeviceWaitIdle(Devices::logical);

    destroySwapchain();

    auto success = createSwapchain();

    if (success) {
        Imgui::recalculateScale(state);
    }

    return success;
}

bool Swapchain::createSwapchain() {
   log::d("Creating Swapchain");

    Swapchain::SwapchainSupportDetails swapchainSupport = Swapchain::querySwapchainSupport(
            Devices::physical);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
    VkPresentModeKHR presentModeTemp = chooseSwapPresentMode(swapchainSupport.presentModes);
    VkExtent2D extentTemp = chooseSwapExtent(swapchainSupport.capabilities);

    if (extentTemp.width < 1 || extentTemp.height < 1) {
       log::v("Invalid swapchain extents. Retry later!");
        Swapchain::needsNewSwapchain = true;
        return false;
    }

    // One more image than the minimum to avoid stalling if the driver is still working on the image
    Swapchain::minImageCount = swapchainSupport.capabilities.minImageCount + 1;
    if (swapchainSupport.capabilities.maxImageCount > 0 &&
        Swapchain::minImageCount > swapchainSupport.capabilities.maxImageCount) {
        Swapchain::minImageCount = swapchainSupport.capabilities.maxImageCount;
    }
   log::v("Creating the swapchain with at least", Swapchain::minImageCount, "images!");

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = Swapchain::surface;
    createInfo.minImageCount = Swapchain::minImageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extentTemp;
    createInfo.imageArrayLayers = 1; // Can be 2 for 3D, etc.
    // TODO switch to VK_IMAGE_USAGE_TRANSFER_DST_BIT for post processing, instead of directly rendering to the SC
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueIndices[] = {Devices::queueFamilyIndices.graphicsFamily.value(),
                               Devices::queueFamilyIndices.presentFamily.value()};

    if (!Devices::queueFamilyIndices.isUnifiedGraphicsPresentQueue()) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // Image is shared between queues -> no transfers!
        createInfo.queueFamilyIndexCount = 2; // Concurrent mode requires at least two indices
        createInfo.pQueueFamilyIndices = queueIndices; // Share image between these queues
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Image is owned by one queue at a time -> Perf+
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    // Do not add any swapchain transforms beyond the default
    createInfo.preTransform = swapchainSupport.capabilities.currentTransform;

    // Do not blend with other windows
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentModeTemp;

    // Clip pixels if obscured by other window -> Perf+
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = nullptr; // Put previous swapchain here if overridden, e.g. if window size changed

    if (vkCreateSwapchainKHR(Devices::logical, &createInfo, nullptr, &Swapchain::swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swapchain!");
    }

    // imageCount only specified a minimum!
    vkGetSwapchainImagesKHR(Devices::logical, Swapchain::swapchain, &Swapchain::imageCount, nullptr);
    Swapchain::images.resize(Swapchain::imageCount);
    vkGetSwapchainImagesKHR(Devices::logical, Swapchain::swapchain, &Swapchain::imageCount,
                            Swapchain::images.data());
    Swapchain::imageFormat = surfaceFormat.format;
    Swapchain::extent = extentTemp;
    Swapchain::presentMode = presentModeTemp;

    Swapchain::createImageViews();
    Swapchain::createDepthResources();
    RenderPasses::create();
    createFramebuffers();

    return true;
}

void Swapchain::destroySwapchain() {
   log::d("Destroying Swapchain");

    for (auto &swapchainFramebuffer: Swapchain::framebuffers) {
        vkDestroyFramebuffer(Devices::logical, swapchainFramebuffer, nullptr);
    }

    RenderPasses::destroy();

    vkDestroyImageView(Devices::logical, Swapchain::depthImageView, nullptr);
    vkDestroyImage(Devices::logical, Swapchain::depthImage, nullptr);
    vkFreeMemory(Devices::logical, Swapchain::depthImageMemory, nullptr);

    for (auto &swapchainImageView: Swapchain::imageViews) {
        vkDestroyImageView(Devices::logical, swapchainImageView, nullptr);
    }

    vkDestroySwapchainKHR(Devices::logical, Swapchain::swapchain, nullptr);
}

void Swapchain::createImageViews() {
    Swapchain::imageViews.resize(Swapchain::images.size());

    for (uint32_t i = 0; i < Swapchain::images.size(); ++i) {
        Swapchain::imageViews[i] = Images::createImageView(Swapchain::images[i],
                                                           Swapchain::imageFormat);
    }
}

void Swapchain::createDepthResources() {
    VkFormat depthFormat = findDepthFormat();

    Images::createImage(Swapchain::extent.width,
                        Swapchain::extent.height,
                        depthFormat,
                        VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        Swapchain::depthImage,
                        Swapchain::depthImageMemory);
    Swapchain::depthImageView = Images::createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

VkFormat Swapchain::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                        VkFormatFeatureFlags features) {
    for (VkFormat format: candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(Devices::physical, format, &props);

        if ((tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) ||
            (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)) {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported format!");
}

VkFormat Swapchain::findDepthFormat() {
    return findSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL, // -> More efficient https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImageTiling.html
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT // -> Can be used as depth/stencil attachment & input attachment
    );
}

bool Swapchain::hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void Swapchain::createFramebuffers() {
    Swapchain::framebuffers.resize(Swapchain::imageViews.size());

    for (size_t i = 0; i < Swapchain::imageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = {};
        attachments[0] = Swapchain::imageViews[i];
        attachments[1] = Swapchain::depthImageView; // Always write to the same depth image. YOLO

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = RenderPasses::renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = Swapchain::extent.width;
        framebufferInfo.height = Swapchain::extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(Devices::logical, &framebufferInfo, nullptr,
                                &Swapchain::framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer!");
        }
    }
}

bool Swapchain::shouldRecreateSwapchain() {
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    bool framebufferChanged = w != Swapchain::framebufferWidth || h != Swapchain::framebufferHeight;

    return Swapchain::needsNewSwapchain || framebufferChanged;
}