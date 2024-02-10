//
// Created by Sam on 2024-01-23.
//

#ifndef DOUGHNUT_RENDER_API_H
#define DOUGHNUT_RENDER_API_H

#include "preprocessor.h"
#include "io/window.h"
#include "graphics/vulkan/optional_features.h"
#include "graphics/vulkan/queue_family_indices.h"
#include "graphics/vulkan/swapchain.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <string>
#include <optional>

namespace dn {
    class VulkanAPI {
    public:
        VulkanAPI(Window &window);

        ~VulkanAPI();

    private:
        void createInstance(const std::string &title);

        void createDevice();

        void createSwapchain();

        void destroySwapchain();

        void destroyDevice();

        void destroyInstance();

        Window &mWindow;
        vk::Instance mInstance = nullptr;
        vk::SurfaceKHR mSurface = nullptr;

        vk::PhysicalDevice mPhysicalDevice = nullptr;
        vk::Device mDevice = nullptr;
        vulkan::OptionalFeatures mOptionalFeatures{};
        vulkan::QueueFamilyIndices mQueueFamilyIndices{};

        vk::Queue mGraphicsQueue = nullptr;
        vk::Queue mPresentQueue = nullptr;

        std::optional<vulkan::Swapchain> mSwapchain{};
    };
}

#endif //DOUGHNUT_RENDER_API_H
