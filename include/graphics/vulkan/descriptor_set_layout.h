//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUT_DESCRIPTOR_SET_LAYOUT_H
#define DOUGHNUT_DESCRIPTOR_SET_LAYOUT_H

#include <vulkan/vulkan.hpp>
#include <vector>
#include "instance.h"

namespace dn::vulkan {
    struct DescriptorSetLayoutConfiguration {
    };

    class DescriptorSetLayout {
    public:
        DescriptorSetLayout(Instance &instance, DescriptorSetLayoutConfiguration config);

        DescriptorSetLayout(DescriptorSetLayout &&other) noexcept;

        ~DescriptorSetLayout();

        vk::DescriptorSetLayout mLayout = nullptr;

    private:
        Instance &mInstance;
    };
}

#endif //DOUGHNUT_DESCRIPTOR_SET_LAYOUT_H
