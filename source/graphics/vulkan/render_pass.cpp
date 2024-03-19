//
// Created by Sam on 2024-02-11.
//

#include "graphics/vulkan/render_pass.h"
#include "io/logger.h"
#include "graphics/vulkan/image.h"

using namespace dn;
using namespace dn::vulkan;

RenderPass::RenderPass(Context &context, const RenderPassConfiguration &config)
        : Handle<vk::RenderPass, RenderPassConfiguration>(context, config) {

    // Color attachment
    vk::AttachmentDescription colorAttachment{
            {},
            config.surfaceFormat,
            vk::SampleCountFlagBits::e1, // MSAA
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eStore,
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::ePresentSrcKHR,
    };

    vk::AttachmentReference colorAttachmentRef{
            0, vk::ImageLayout::eColorAttachmentOptimal
    };

    // Depth attachment
    vk::AttachmentDescription depthAttachment{
            {},
            findDepthFormat(mContext.mPhysicalDevice),
            vk::SampleCountFlagBits::e1, // MSAA
            vk::AttachmentLoadOp::eClear,
            vk::AttachmentStoreOp::eDontCare,
            vk::AttachmentLoadOp::eDontCare,
            vk::AttachmentStoreOp::eDontCare,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    vk::AttachmentReference depthAttachmentRef{
            1, vk::ImageLayout::eDepthStencilAttachmentOptimal
    };

    // Single subpass
    vk::SubpassDescription subpassDescription{
            {},
            vk::PipelineBindPoint::eGraphics,
            0, nullptr,
            1, &colorAttachmentRef,
            nullptr,
            &depthAttachmentRef,
            0, nullptr
    };

    std::array<vk::AttachmentDescription, 2> attachments{
            colorAttachment, depthAttachment
    };

    // To avoid layout transitions before the image has been acquired
    vk::SubpassDependency dependency{
            vk::SubpassExternal,
            0, // dstSubpass > srcSubpass !!! (unless VK_SUBPASS_EXTERNAL)
            {vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests},
            {vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests},
            {},
            {vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite}
    };

    vk::RenderPassCreateInfo renderPassCreateInfo{
            {},
            static_cast<uint32_t>(attachments.size()), attachments.data(),
            1, &subpassDescription,
            1, &dependency
    };

    mVulkan = mContext.mDevice.createRenderPass(renderPassCreateInfo);
}

RenderPass::~RenderPass() {
    if (mVulkan != nullptr) { mContext.mDevice.destroy(mVulkan); }
}