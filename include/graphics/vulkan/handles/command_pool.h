//
// Created by Sam on 2024-02-12.
//

#ifndef DOUGHNUT_COMMAND_POOL_H
#define DOUGHNUT_COMMAND_POOL_H

#include "graphics/vulkan/context.h"
#include "graphics/vulkan/handle.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct CommandPoolConfiguration {
        uint32_t queueFamilyIndex;
    };

    class CommandPool : public Handle<vk::CommandPool, CommandPoolConfiguration> {
    public:
        CommandPool(Context &context,
                    const CommandPoolConfiguration &config);

        CommandPool(CommandPool &&other) = default;

        ~CommandPool();
    };
}

#endif //DOUGHNUT_COMMAND_POOL_H
