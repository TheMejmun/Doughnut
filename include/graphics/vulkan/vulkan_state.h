//
// Created by Saman on 26.08.23.
//

#ifndef DOUGHNUT_VULKAN_STATE_H
#define DOUGHNUT_VULKAN_STATE_H

#include "preprocessor.h"

#include <vulkan/vulkan.h>

namespace Doughnut::Graphics::Vk {
    struct State {
        VkCommandPool commandPool = nullptr;
    };
}

#endif //DOUGHNUT_VULKAN_STATE_H
