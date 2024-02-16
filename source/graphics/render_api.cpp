//
// Created by Sam on 2024-01-23.
//

#include "graphics/render_api.h"
#include "io/logger.h"
#include "util/require.h"

#include <stdexcept>
#include <vector>
#include <set>

using namespace dn;
using namespace dn::vulkan;

VulkanAPI::VulkanAPI(Window &window) {
    log::d("Creating VulkanAPI");

    mInstance.emplace(
            window,
            InstanceConfiguration{}
    );

    mSwapchain.emplace(
            *mInstance,
            SwapchainConfiguration{false}
    );

    mVertexBuffer.emplace(
            *mInstance,
            BufferConfiguration{VERTEX, false}
    );
    mIndexBuffer.emplace(
            *mInstance,
            BufferConfiguration{INDEX, false}
    );
    // TODO one per frame in flight
    mUniformBuffer.emplace(
            *mInstance,
            BufferConfiguration{UNIFORM, true}
    );

    mCommandPool.emplace(
            *mInstance
    );

    // if (!(*mSwapchain).shouldRecreate()) {
    // TODO do we need this condition?
    mPipeline.emplace(
            *mInstance,
            *mSwapchain->mRenderPass,
            *mUniformBuffer,
            PipelineConfiguration{}
    );

    for (uint32_t i = 0; i < mSwapchain->getImageCount(); ++i) {
        mCommandBuffers.emplace_back(
                *mInstance,
                *mCommandPool
        );
    }
    // }

    mImageAvailableSemaphore.emplace(
            *mInstance
    );
    mRenderFinishedSemaphore.emplace(
            *mInstance
    );
    mInFlightFence.emplace(
            *mInstance,
            FenceConfiguration{true}
    );
}

bool VulkanAPI::nextImage() {
    mInFlightFence->await();

    if (mSwapchain->shouldRecreate()) {
        log::d("Requesting swapchain recreation");
        mSwapchain->recreate();
    }

    auto acquireImageResult = mSwapchain->acquireNextImage(*mImageAvailableSemaphore);

    mInFlightFence->resetFence();

    if (!acquireImageResult.has_value()) {
        log::d("No swapchain image was acquired. Skipping frame.");
        mCurrentSwapchainFramebuffer.reset();
        return false;
    } else {
        // log::d("mCurrentSwapchainFramebuffer", acquireImageResult.value());
        mCurrentSwapchainFramebuffer = acquireImageResult;
        return true;
    }
}

void VulkanAPI::startRecording() {
    debugRequire(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");
    mCommandBuffers[*mCurrentSwapchainFramebuffer].startRecording();
}

void VulkanAPI::beginRenderPass() {
    debugRequire(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");

    std::array<vk::ClearValue, 2> clearValues{
            vk::ClearValue{{1.0f, 0.0f, 0.0f, 1.0f}},
            vk::ClearValue{{1.0f, 0}}
    };
    vk::RenderPassBeginInfo renderPassInfo{
            mSwapchain->mRenderPass->mRenderPass,
            mSwapchain->getFramebuffer(*mCurrentSwapchainFramebuffer).mFramebuffer,
            vk::Rect2D{
                    {0,                      0},
                    {mSwapchain->getWidth(), mSwapchain->getHeight()}
            },
            static_cast<uint32_t>(clearValues.size()),
            clearValues.data()
    };

    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void VulkanAPI::endRenderPass() {
    debugRequire(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");
    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.endRenderPass();
}

void VulkanAPI::endRecording() {
    debugRequire(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");
    mCommandBuffers[*mCurrentSwapchainFramebuffer].endRecording();
}

void VulkanAPI::recordMeshDraw(const vulkan::BufferPosition &vertexPosition,
                               const vulkan::BufferPosition &indexPosition) {
    debugRequire(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");

    // TODO put this somewhere reasonable
    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline->mGraphicsPipeline);

    std::array<vk::Buffer, 1> vertexBuffers{mVertexBuffer->mBuffer};
    std::array<vk::DeviceSize, 1> offsets{0};
    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.bindVertexBuffers(
            0,
            1,
            vertexBuffers.data(),
            offsets.data()
    );

    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.bindIndexBuffer(
            mIndexBuffer->mBuffer,
            0,
            vk::IndexType::eUint32
    );

    vk::Viewport viewport{
            -1.0f,
            -1.0f,
            2.0f,
            2.0f,
            0.0f,
            1.0f
    };

//    vk::Viewport viewport{
//            0.0f,
//            0.0f,
//            static_cast<float>(mSwapchain->getWidth()),
//            static_cast<float>(mSwapchain->getHeight()),
//            0.0f,
//            1.0f
//    };

    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.setViewport(
            0,
            1,
            &viewport
    );

    vk::Rect2D scissor{
            {0, 0},
            {mSwapchain->getWidth(), mSwapchain->getHeight()}
    };

    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.setScissor(
            0,
            1,
            &scissor
    );

    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            mPipeline->mPipelineLayout,
            0,
            1,
            mPipeline->mDescriptorSet->mDescriptorSets.data(), // TODO don't just pass all of this in here
            0,
            nullptr
    );

    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.drawIndexed(
            indexPosition.count,
            1,
            indexPosition.memoryIndex / sizeof(uint32_t),
            vertexPosition.memoryIndex / sizeof(uint32_t),
            0
    );

    // TODO this->drawUi(ecs);
}

void VulkanAPI::drawFrame(double delta) {
    debugRequire(mCurrentSwapchainFramebuffer.has_value(), "Can not draw if no image has been acquired.");
//    uploadRenderables(ecs);
//    uploadSimplifiedMeshes(ecs);
//    destroyRenderables(ecs);

    // mInFlightFence->resetFence();

    // TODO updateUniformBuffer(delta, ecs);

    // or VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT

    std::array<vk::Semaphore, 1> waitSemaphores{mImageAvailableSemaphore->mSemaphore}; // index corresponding to wait stage
    std::array<vk::PipelineStageFlags, 1> waitStages{vk::PipelineStageFlagBits::eColorAttachmentOutput}; // Wait in fragment stage
    std::array<vk::Semaphore, 1> signalSemaphores{mRenderFinishedSemaphore->mSemaphore};
    vk::SubmitInfo submitInfo{
            static_cast<uint32_t>(waitSemaphores.size()),
            waitSemaphores.data(),
            waitStages.data(),
            1,
            &mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer,
            static_cast<uint32_t>(signalSemaphores.size()),
            signalSemaphores.data(),
    };

    mInstance->mGraphicsQueue.submit(submitInfo, mInFlightFence->mFence);

    vk::PresentInfoKHR presentInfo{
            static_cast<uint32_t>(signalSemaphores.size()),
            signalSemaphores.data(),
            1,
            &mSwapchain->mSwapchain,
            &(*mCurrentSwapchainFramebuffer),
            nullptr
    };
    auto result = mInstance->mPresentQueue.presentKHR(presentInfo);

    require(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR, "An error has occured while rendering");

    mCurrentSwapchainFramebuffer.reset();
}

VulkanAPI::~VulkanAPI() {
    log::d("Destroying VulkanAPI");
    mInFlightFence.reset();
    mImageAvailableSemaphore.reset();
    mRenderFinishedSemaphore.reset();

    mCommandBuffers.clear();
    mCommandPool.reset();
    mUniformBuffer.reset();
    mVertexBuffer.reset();
    mIndexBuffer.reset();
    mPipeline.reset();
    mSwapchain.reset();
    mInstance.reset();
}