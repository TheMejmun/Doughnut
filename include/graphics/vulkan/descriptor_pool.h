//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUTSANDBOX_DESCRIPTOR_POOL_H
#define DOUGHNUTSANDBOX_DESCRIPTOR_POOL_H

#include <vulkan/vulkan.hpp>
#include <vector>
#include "instance.h"

namespace dn::vulkan {
    struct DescriptorPoolConfiguration {
        uint32_t size;
    };

    class DescriptorPool {
    public:
        DescriptorPool(Instance &instance, DescriptorPoolConfiguration config);

        DescriptorPool(DescriptorPool &&other) noexcept;

        ~DescriptorPool();

        vk::DescriptorPool mDescriptorPool = nullptr;

    private:
        Instance& mInstance;
    };
}

#endif //DOUGHNUTSANDBOX_DESCRIPTOR_POOL_H