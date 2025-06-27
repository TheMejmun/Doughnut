//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUT_DESCRIPTOR_POOL_H
#define DOUGHNUT_DESCRIPTOR_POOL_H

#include "graphics/vulkan/context.h"
#include "graphics/vulkan/handle.h"

#include <vulkan/vulkan.hpp>
#include <vector>

namespace dn::vulkan {
    enum DescriptorType {
        UNIFORM_BUFFER,
        SAMPLER // eCombinedImageSampler TODO differentiate different sampler types
    };

    struct DescriptorPoolSize {
        DescriptorType type;
        uint32_t count;
    };

    struct DescriptorPoolConfiguration {
        uint32_t maxFramesInFlight; // TODO set to Frames in flight
        std::vector<DescriptorPoolSize> sizes;
        bool freeable = false;
    };

    class DescriptorPool : public Handle<vk::DescriptorPool, DescriptorPoolConfiguration> {
    public:
        DescriptorPool(Context &context, const DescriptorPoolConfiguration &config);

        DescriptorPool(DescriptorPool &&other) = default;

        ~DescriptorPool();
    };
}

#endif //DOUGHNUT_DESCRIPTOR_POOL_H
