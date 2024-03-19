//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUT_DESCRIPTOR_POOL_H
#define DOUGHNUT_DESCRIPTOR_POOL_H

#include <vulkan/vulkan.hpp>
#include <vector>
#include "context.h"

namespace dn::vulkan {
    enum DescriptorType{
        UNIFORM_BUFFER,
        SAMPLER // eCombinedImageSampler TODO differentiate different sampler types
    };

    struct DescriptorPoolSize{
        DescriptorType type;
        uint32_t count;
    };

    struct DescriptorPoolConfiguration {
        uint32_t maxFramesInFlight; // TODO set to Frames in flight
        std::vector<DescriptorPoolSize> sizes;
        bool freeable = false;
    };

    class DescriptorPool {
    public:
        DescriptorPool(Context &context, const DescriptorPoolConfiguration& config);

        DescriptorPool(DescriptorPool &&other) noexcept;

        ~DescriptorPool();

        vk::DescriptorPool mDescriptorPool = nullptr;

    private:
        Context& mContext;
    };
}

#endif //DOUGHNUT_DESCRIPTOR_POOL_H
