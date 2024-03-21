//
// Created by Sam on 2024-02-10.
//

#ifndef DOUGHNUT_INSTANCE_H
#define DOUGHNUT_INSTANCE_H

#include "io/window.h"
#include "optional_features.h"
#include "queue_family_indices.h"

#include <vulkan/vulkan.hpp>
#include <string>

namespace dn::vulkan {
    struct ContextConfiguration {
    };

    class Context {
    public:
        Context(Window &window, ContextConfiguration config);

        Context(const Context &other) = delete;

        Context(Context &&other) = default;

        ~Context();

        Window &mWindow;

        vk::Instance mInstance = nullptr;
        vk::SurfaceKHR mSurface = nullptr;
        vk::PhysicalDevice mPhysicalDevice = nullptr;
        vk::Device mDevice = nullptr;

        // TODO move Queue creation somewhere else?
        vk::Queue mGraphicsQueue = nullptr;
        vk::Queue mPresentQueue = nullptr;
        vk::Queue mTransferQueue = nullptr;

        vulkan::OptionalFeatures mOptionalFeatures{};
        vulkan::QueueFamilyIndices mQueueFamilyIndices{};
    };
}

#endif //DOUGHNUT_INSTANCE_H
