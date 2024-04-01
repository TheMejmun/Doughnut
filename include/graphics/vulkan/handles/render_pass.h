//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUT_RENDER_PASS_H
#define DOUGHNUT_RENDER_PASS_H

#include "graphics/vulkan/context.h"
#include "graphics/vulkan/handle.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct RenderPassConfiguration {
        vk::Format surfaceFormat;
    };

    class RenderPass : public Handle<vk::RenderPass, RenderPassConfiguration> {
    public:
        RenderPass(Context &context,
                   const RenderPassConfiguration& config);

        RenderPass(RenderPass &&other) = default;

        ~RenderPass();
    };
}

#endif //DOUGHNUT_RENDER_PASS_H
