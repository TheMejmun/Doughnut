//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUT_SEMAPHORE_H
#define DOUGHNUT_SEMAPHORE_H

#include "graphics/vulkan/context.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    class Semaphore {
    public:
        Semaphore(Context &context);

        Semaphore(Semaphore &&other) noexcept;

        ~Semaphore();

        vk::Semaphore mSemaphore = nullptr;

    private:
        Context &mContext;
    };
}

#endif //DOUGHNUT_SEMAPHORE_H
