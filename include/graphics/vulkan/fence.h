//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUT_FENCE_H
#define DOUGHNUT_FENCE_H

#include "graphics/vulkan/context.h"
#include "handle.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct FenceConfiguration {
        bool startSignalled;
    };

    class Fence : public Handle<vk::Fence, FenceConfiguration> {
    public:
        Fence(Context &context,
              FenceConfiguration config);

        Fence(Fence &&other) = default;

        ~Fence();

        double await() const;

        void resetFence() const;

        [[nodiscard]] bool isWaiting() const;
    };
}

#endif //DOUGHNUT_FENCE_H
