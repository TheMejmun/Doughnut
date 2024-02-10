//
// Created by Sam on 2024-02-10.
//

#ifndef DOUGHNUTSANDBOX_FRAMEBUFFER_H
#define DOUGHNUTSANDBOX_FRAMEBUFFER_H

#include <vulkan/vulkan.hpp>
#include "image_view.h"

namespace dn::vulkan {
    class Framebuffer {
    public:
        Framebuffer(vk::Device device,
                    const std::vector<ImageView*> &attachments,
                    vk::RenderPass renderPass);

        Framebuffer(Framebuffer &&other) noexcept;

        ~Framebuffer();

        vk::Framebuffer mFramebuffer = nullptr;
    private:
        vk::Device mDevice= nullptr;
    };
}
#endif //DOUGHNUTSANDBOX_FRAMEBUFFER_H
