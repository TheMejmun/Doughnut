//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUTSANDBOX_SEMAPHORE_H
#define DOUGHNUTSANDBOX_SEMAPHORE_H

#include "graphics/vulkan/instance.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    class Semaphore {
    public:
        Semaphore(Instance &instance);

        Semaphore(Semaphore &&other) noexcept;

        ~Semaphore();

        vk::Semaphore mSemaphore = nullptr;

    private:
        Instance &mInstance;
    };
}

#endif //DOUGHNUTSANDBOX_SEMAPHORE_H
