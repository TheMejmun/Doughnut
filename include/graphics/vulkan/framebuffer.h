//
// Created by Sam on 2024-02-10.
//

#ifndef DOUGHNUTSANDBOX_FRAMEBUFFER_H
#define DOUGHNUTSANDBOX_FRAMEBUFFER_H

#include "image_view.h"
#include "instance.h"

#include <vulkan/vulkan.hpp>

namespace dn::vulkan {
    class Framebuffer {
    public:
        Framebuffer(Instance &instance,
                    const std::vector<ImageView *> &attachments,
                    vk::RenderPass renderPass);

        Framebuffer(Framebuffer &&other) noexcept;

        ~Framebuffer();

        vk::Framebuffer mFramebuffer = nullptr;

    private:
        Instance &mInstance;
    };
}
#endif //DOUGHNUTSANDBOX_FRAMEBUFFER_H
