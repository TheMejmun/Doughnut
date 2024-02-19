//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUT_DESCRIPTOR_SET_H
#define DOUGHNUT_DESCRIPTOR_SET_H

#include <vulkan/vulkan.hpp>
#include <vector>
#include "instance.h"
#include "descriptor_set_layout.h"
#include "descriptor_pool.h"
#include "buffer.h"

namespace dn::vulkan {
    struct DescriptorSetConfiguration {
        uint32_t setCount;
        uint32_t uboSize;
    };

    class DescriptorSet {
    public:
        DescriptorSet(Instance &instance,
                      Buffer &uboBuffer,
                      DescriptorSetLayout &layout,
                      DescriptorPool &pool,
                      DescriptorSetConfiguration config);

        DescriptorSet(DescriptorSet &&other) noexcept;

        ~DescriptorSet();

        std::vector<vk::DescriptorSet> mDescriptorSets{};
        std::vector<UploadResult> mBufferIndices{};

    private:
        Instance &mInstance;
        Buffer &mUboBuffer;
    };
}

#endif //DOUGHNUT_DESCRIPTOR_SET_H
