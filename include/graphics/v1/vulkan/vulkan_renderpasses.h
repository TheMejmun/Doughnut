//
// Created by Saman on 24.08.23.
//

#ifndef DOUGHNUT_VULKAN_RENDERPASSES_H
#define DOUGHNUT_VULKAN_RENDERPASSES_H

#include "preprocessor.h"

#include <vulkan/vulkan.h>

namespace Doughnut::Graphics::Vk::RenderPasses{
    extern VkRenderPass renderPass;

    void create();
    void destroy();
}

#endif //DOUGHNUT_VULKAN_RENDERPASSES_H
