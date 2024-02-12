//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUTSANDBOX_COMMAND_POOL_H
#define DOUGHNUTSANDBOX_COMMAND_POOL_H

#include "instance.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    class CommandPool {
    public:
        CommandPool(Instance &instance);

        CommandPool(CommandPool &&other) noexcept;

        ~CommandPool();

        vk::CommandPool mCommandPool = nullptr;

    private:
        Instance &mInstance;
    };
}

#endif //DOUGHNUTSANDBOX_COMMAND_POOL_H
