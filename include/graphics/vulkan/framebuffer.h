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
    class Framebuffer {
    public:
        Framebuffer(Context &context,
                    const std::vector<ImageView *> &attachments,
                    RenderPass &renderPass);

        Framebuffer(Framebuffer &&other) noexcept;

        ~Framebuffer();

        vk::Framebuffer mFramebuffer = nullptr;

    private:
        Context &mContext;
    };
}
#endif //DOUGHNUT_FRAMEBUFFER_H
