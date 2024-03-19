//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUT_DESCRIPTOR_SET_H
#define DOUGHNUT_DESCRIPTOR_SET_H

#include "context.h"
#include "descriptor_set_layout.h"
#include "descriptor_pool.h"
#include "buffer.h"
#include "sampler.h"
#include "image_view.h"

#include <vulkan/vulkan.hpp>
#include <vector>

namespace dn::vulkan {
    // TODO These objects being inside the config object feels wrong
    struct DescriptorSetConfiguration {
        uint32_t setCount;
        uint32_t uboSize;
        Buffer &uboBuffer;
        Sampler &sampler;
        ImageView &imageView;
    };

    class DescriptorSet {
    public:
        DescriptorSet(Context &context,
                      DescriptorSetLayout &layout,
                      DescriptorPool &pool,
                      const DescriptorSetConfiguration &config);

        DescriptorSet(DescriptorSet &&other) noexcept;

        ~DescriptorSet();

        std::vector<vk::DescriptorSet> mDescriptorSets{};
        std::vector<UploadResult> mBufferIndices{};

    private:
        Context &mContext;
        DescriptorSetConfiguration mConfig;
    };
}

#endif //DOUGHNUT_DESCRIPTOR_SET_H
