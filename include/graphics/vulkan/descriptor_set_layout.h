//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUT_DESCRIPTOR_SET_LAYOUT_H
#define DOUGHNUT_DESCRIPTOR_SET_LAYOUT_H

#include "context.h"
#include "handle.h"

#include <vulkan/vulkan.hpp>
#include <vector>

namespace dn::vulkan {
    struct DescriptorSetLayoutConfiguration {
    };

    class DescriptorSetLayout : public Handle<vk::DescriptorSetLayout, DescriptorSetLayoutConfiguration> {
    public:
        DescriptorSetLayout(Context &context, const DescriptorSetLayoutConfiguration &config);

        DescriptorSetLayout(DescriptorSetLayout &&other) = default;

        ~DescriptorSetLayout();
    };
}

#endif //DOUGHNUT_DESCRIPTOR_SET_LAYOUT_H
