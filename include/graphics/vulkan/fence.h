//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUTSANDBOX_FENCE_H
#define DOUGHNUTSANDBOX_FENCE_H

#include "graphics/vulkan/instance.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct FenceConfiguration {
        bool startSignalled;
    };

    class Fence {
    public:
        Fence(Instance &instance,
              FenceConfiguration config);

        Fence(Fence &&other) noexcept;

        ~Fence();

        double await() const;

        void resetFence() const;

        vk::Fence mFence = nullptr;

    private:
        Instance &mInstance;
    };
}

#endif //DOUGHNUTSANDBOX_FENCE_H
