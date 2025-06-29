//
// Created by Sam on 2024-02-10.
//

#include "graphics/vulkan/handles/framebuffer.h"
#include "io/logger.h"
#include "util/require.h"

using namespace dn;
using namespace dn::vulkan;


Framebuffer::Framebuffer(Context &context,
                         const std::vector<ImageView *> &attachments,
                         RenderPass &renderPass,
                         const FramebufferConfiguration &config)
        : Handle<vk::Framebuffer, FramebufferConfiguration>(context, config) {

    require(!attachments.empty(), "Can not create a Framebuffer on empty attachments");

    std::vector<vk::ImageView> data{};
    data.reserve(attachments.size());
    for (const auto imageView: attachments) {
        data.push_back(**imageView);
    }

    vk::FramebufferCreateInfo createInfo{
            {},
            *renderPass,
            static_cast<uint32_t>(data.size()), data.data(),
            attachments[0]->mConfig.extent.width,
            attachments[0]->mConfig.extent.height,
            1
    };

    mVulkan = mContext.mDevice.createFramebuffer(createInfo);

    this->registerDebug();
}

Framebuffer::~Framebuffer() {
    if (mVulkan != nullptr) { mContext.mDevice.destroy(mVulkan); }
}