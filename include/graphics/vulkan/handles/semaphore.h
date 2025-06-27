//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUT_SEMAPHORE_H
#define DOUGHNUT_SEMAPHORE_H

#include "graphics/vulkan/context.h"
#include "graphics/vulkan/handle.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct SemaphoreConfiguration {
    };

    class Semaphore : public Handle<vk::Semaphore, SemaphoreConfiguration> {
    public:
        Semaphore(Context &context, const SemaphoreConfiguration &config);

        Semaphore(Semaphore &&other) = default;

        ~Semaphore();
    };
}

#endif //DOUGHNUT_SEMAPHORE_H
