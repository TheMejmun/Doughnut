//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUTSANDBOX_RENDER_PASS_H
#define DOUGHNUTSANDBOX_RENDER_PASS_H

#include "graphics/vulkan/instance.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct RenderPassConfiguration {
        vk::Format surfaceFormat;
        vk::Format depthFormat;
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

#endif //DOUGHNUTSANDBOX_RENDER_PASS_H
