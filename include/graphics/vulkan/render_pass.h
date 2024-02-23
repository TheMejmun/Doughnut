//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUT_RENDER_PASS_H
#define DOUGHNUT_RENDER_PASS_H

#include "graphics/vulkan/instance.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct RenderPassConfiguration {
        vk::Format surfaceFormat;
    };

    class RenderPass {
    public:
        RenderPass(Instance &instance,
                   RenderPassConfiguration config);

        RenderPass(RenderPass &&other) noexcept;

        ~RenderPass();

        vk::RenderPass mRenderPass = nullptr;

    private:
        Instance &mInstance;
    };
}

#endif //DOUGHNUT_RENDER_PASS_H
