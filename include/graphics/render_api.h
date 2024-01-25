//
// Created by Sam on 2024-01-23.
//

#ifndef DOUGHNUT_RENDER_API_H
#define DOUGHNUT_RENDER_API_H

#include "preprocessor.h"
#include "queue_family_indices.h"
#include "optional_features.h"
#include "io/window.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <string>

namespace Doughnut {
    class VulkanAPI {
    public:
        VulkanAPI(Window *window, const std::string &title);

        ~VulkanAPI();

    private:
        void createInstance(const std::string &title);

        void createDevice();

        void destroyDevice();

        void destroyInstance();

        Window *mWindow;
        vk::Instance mInstance = nullptr;
        vk::SurfaceKHR mSurface = nullptr;

        vk::PhysicalDevice mPhysicalDevice = nullptr;
        vk::Device mDevice = nullptr;
        OptionalFeatures mOptionalFeatures{};
        QueueFamilyIndices mQueueFamilyIndices{};

        vk::Queue mGraphicsQueue = nullptr;
        vk::Queue mPresentQueue = nullptr;
    };
}

#endif //DOUGHNUT_RENDER_API_H
