//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUT_DESCRIPTOR_SET_H
#define DOUGHNUT_DESCRIPTOR_SET_H

#include "instance.h"
#include "descriptor_set_layout.h"
#include "descriptor_pool.h"
#include "buffer.h"
#include "sampler.h"
#include "image_view.h"

#include <vulkan/vulkan.hpp>
#include <vector>

namespace dn::vulkan {
    struct DescriptorSetConfiguration {
        uint32_t setCount;
        uint32_t uboSize;
        Buffer &uboBuffer;
        Sampler &sampler;
        ImageView &imageView;
    };

    class DescriptorSet {
    public:
        DescriptorSet(Instance &instance,
                      DescriptorSetLayout &layout,
                      DescriptorPool &pool,
                      const DescriptorSetConfiguration &config);

        DescriptorSet(DescriptorSet &&other) noexcept;

        ~DescriptorSet();

        std::vector<vk::DescriptorSet> mDescriptorSets{};
        std::vector<UploadResult> mBufferIndices{};

    private:
        Instance &mInstance;
        DescriptorSetConfiguration mConfig;
    };
}

#endif //DOUGHNUT_DESCRIPTOR_SET_H
