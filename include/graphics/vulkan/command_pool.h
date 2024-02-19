//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUTSANDBOX_COMMAND_POOL_H
#define DOUGHNUTSANDBOX_COMMAND_POOL_H

#include "instance.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct CommandPoolConfiguration {
        uint32_t queueFamilyIndex;
    };

    class CommandPool {
    public:
        CommandPool(Instance &instance, CommandPoolConfiguration config);

        CommandPool(CommandPool &&other) noexcept;

        ~CommandPool();

        vk::CommandPool mCommandPool = nullptr;

    private:
        Instance &mInstance;
    };
}

#endif //DOUGHNUTSANDBOX_COMMAND_POOL_H
