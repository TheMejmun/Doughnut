//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUT_COMMAND_POOL_H
#define DOUGHNUT_COMMAND_POOL_H

#include "context.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct CommandPoolConfiguration {
        uint32_t queueFamilyIndex;
    };

    class CommandPool {
    public:
        CommandPool(Context &context, CommandPoolConfiguration config);

        CommandPool(CommandPool &&other) noexcept;

        ~CommandPool();

        vk::CommandPool mCommandPool = nullptr;

    private:
        Context &mContext;
    };
}

#endif //DOUGHNUT_COMMAND_POOL_H
