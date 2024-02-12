//
// Created by Sam on 2024-02-10.
//

#ifndef DOUGHNUTSANDBOX_INSTANCE_H
#define DOUGHNUTSANDBOX_INSTANCE_H

#include "io/window.h"
#include "optional_features.h"
#include "queue_family_indices.h"

#include <vulkan/vulkan.hpp>
#include <string>

namespace dn::vulkan {
    struct InstanceConfiguration {
    };

    class Instance {
    public:
        Instance(Window &window, InstanceConfiguration config);

        Instance(Instance &&other) noexcept;

        ~Instance();

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

#endif //DOUGHNUTSANDBOX_INSTANCE_H
