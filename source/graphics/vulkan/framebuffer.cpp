//
// Created by Sam on 2024-02-10.
//

#include "graphics/vulkan/framebuffer.h"
#include "io/logger.h"
#include "util/require.h"

using namespace dn;
using namespace dn::vulkan;


Framebuffer::Framebuffer(Context &context,
                         const std::vector<ImageView *> &attachments,
                         RenderPass &renderPass)
        : mContext(context) {
    log::v("Creating Framebuffer");
    require(!attachments.empty(), "Can not create a Framebuffer on empty attachments");

    std::vector<vk::ImageView> data{};
    data.reserve(attachments.size());
    for (const auto imageView: attachments) {
        data.push_back(imageView->mImageView);
    }

    vk::FramebufferCreateInfo createInfo{
            {},
            renderPass.mRenderPass,
            static_cast<uint32_t>(data.size()), data.data(),
            attachments[0]->mExtent.width,
            attachments[0]->mExtent.height,
            1
    };

    mFramebuffer = mContext.mDevice.createFramebuffer(createInfo);
}

Framebuffer::Framebuffer(dn::vulkan::Framebuffer &&other) noexcept
        : mContext(other.mContext), mFramebuffer(std::exchange(other.mFramebuffer, nullptr)) {
    log::v("Moving Framebuffer");
}

Framebuffer::~Framebuffer() {
    log::v("Destroying Framebuffer");
    if (mFramebuffer != nullptr) { mContext.mDevice.destroy(mFramebuffer); }
}