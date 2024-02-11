//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUTSANDBOX_DESCRIPTOR_SET_H
#define DOUGHNUTSANDBOX_DESCRIPTOR_SET_H

#include <vulkan/vulkan.hpp>
#include <vector>
#include "instance.h"

namespace dn::vulkan {
    struct DescriptorSetConfiguration {

    };

    struct DescriptorSetLayoutConfiguration {
        std::vector<DescriptorSetConfiguration> descriptorSetConfigs;
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

    class DescriptorSet {
    public:
        DescriptorSet(DescriptorSetConfiguration config);

        DescriptorSet(DescriptorSet &&other) noexcept;

        ~DescriptorSet();

    private:
    };
}
#endif //DOUGHNUTSANDBOX_DESCRIPTOR_SET_H
