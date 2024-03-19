//
// Created by Sam on 2024-02-10.
//

#ifndef DOUGHNUT_FRAMEBUFFER_H
#define DOUGHNUT_FRAMEBUFFER_H

#include "image_view.h"
#include "context.h"
#include "render_pass.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    struct FramebufferConfiguration {
    };

    class Framebuffer : public Handle<vk::Framebuffer, FramebufferConfiguration> {
    public:
        Framebuffer(Context &context,
                    const std::vector<ImageView *> &attachments,
                    RenderPass &renderPass,
                    const FramebufferConfiguration &config);

        Framebuffer(Framebuffer &&other) = default;

        ~Framebuffer();
    };
}
#endif //DOUGHNUT_FRAMEBUFFER_H
