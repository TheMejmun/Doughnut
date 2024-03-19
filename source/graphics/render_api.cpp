//
// Created by Sam on 2024-01-23.
//

#include "graphics/render_api.h"
#include "io/logger.h"
#include "util/require.h"
#include "graphics/vulkan/image_staging_buffer.h"
#include "graphics/uniform_buffer_object.h"
#include "util/timer.h"
#include "graphics/push_constants_object.h"

#include <stdexcept>
#include <vector>
#include <set>

using namespace dn;
using namespace dn::vulkan;

VulkanAPI::VulkanAPI(Window &window) {
    trace_function

    log::d("Creating VulkanAPI");

    mContext.emplace(
            window,
            ContextConfiguration{}
    );

    mSwapchain.emplace(
            *mContext,
            SwapchainConfiguration{false}
    );

    mMeshes.emplace(
            *mContext
    );
    mTextures.emplace(
            *mContext
    );

    // TODO one per frame in flight
    mUniformBuffer.emplace(
            *mContext,
            BufferConfiguration{UNIFORM, true}
    );

    mCommandPool.emplace(
            *mContext,
            CommandPoolConfiguration{*mContext->mQueueFamilyIndices.graphicsFamily}
    );

    // if (!(*mSwapchain).shouldRecreate()) {
    // TODO do we need this condition?

    mPipelines.emplace(
            *mContext,
            *mSwapchain->mRenderPass,
            PipelineCacheConfiguration{1u}
    );

    for (uint32_t i = 0; i < mSwapchain->mImageCount; ++i) {
        mCommandBuffers.emplace_back(
                *mContext,
                *mCommandPool
        );
    }
    // }

    mImageAvailableSemaphore.emplace(
            *mContext
    );
    mRenderFinishedSemaphore.emplace(
            *mContext
    );
    mInFlightFence.emplace(
            *mContext,
            FenceConfiguration{true}
    );

    mSampler.emplace(
            *mContext,
            SamplerConfiguration{
                    CLAMP
            }
    );

    mGui.emplace(
            *mContext,
            window,
            *mSwapchain->mRenderPass,
            GuiConfiguration{
                    mSwapchain->mMinImageCount,
                    mSwapchain->mImageCount
            }
    );
    mGui->beginFrame(); // TODO
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
    dnAssert(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");
    mCommandBuffers[*mCurrentSwapchainFramebuffer].startRecording();
}

void VulkanAPI::beginRenderPass() {
    dnAssert(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");

    std::array<vk::ClearValue, 2> clearValues{
            vk::ClearValue{{0.1f, 0.1f, 0.1f, 1.0f}},
            vk::ClearValue{{1.0f, 0}}
    };
    vk::RenderPassBeginInfo renderPassInfo{
            mSwapchain->mRenderPass->mRenderPass,
            mSwapchain->getFramebuffer(*mCurrentSwapchainFramebuffer).mFramebuffer,
            vk::Rect2D{
                    {0, 0},
                    mSwapchain->mExtent
            },
            static_cast<uint32_t>(clearValues.size()),
            clearValues.data()
    };

    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void VulkanAPI::endRenderPass() {
    dnAssert(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");
    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.endRenderPass();
}

void VulkanAPI::endRecording() {
    dnAssert(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");
    mCommandBuffers[*mCurrentSwapchainFramebuffer].endRecording();
}

void VulkanAPI::recordDraw(const Renderable &renderable) {
    dnAssert(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");

    auto &pipeline = mPipelines->get({
                                             renderable.vertexShader,
                                             renderable.fragmentShader,
                                             DescriptorSetConfiguration{
                                                     1u, // TODO this must not be higher than maxFramesInFlight of Descriptor Pool! Crashes otherwise
                                                     sizeof(UniformBufferObject),
                                                     *mUniformBuffer,
                                                     *mSampler,
                                                     mTextures->getImageView(renderable.texture)
                                             },
                                             false
                                     });
    // TODO put this somewhere reasonable
    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                                                               pipeline.mGraphicsPipeline);

    auto &mesh = mMeshes->get(renderable.model);

    // TODO set offsets
    // TODO do indices need to be offset by their in-buffer position?
    std::array<vk::Buffer, 1> vertexBuffers{mesh.vertexBuffer};
    std::array<vk::DeviceSize, 1> offsets{0};
    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.bindVertexBuffers(
            0,
            1,
            vertexBuffers.data(),
            offsets.data()
    );

    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.bindIndexBuffer(
            mesh.indexBuffer,
            0,
            vk::IndexType::eUint32
    );

    vk::Viewport viewport{
            0.0f,
            0.0f,
            static_cast<float>(mSwapchain->mExtent.width),
            static_cast<float>(mSwapchain->mExtent.height),
            0.0f,
            1.0f
    };

    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.setViewport(
            0,
            1,
            &viewport
    );

    vk::Rect2D scissor{
            {0, 0},
            mSwapchain->mExtent
    };

    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.setScissor(
            0,
            1,
            &scissor
    );

    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipeline.mPipelineLayout,
            0,
            1,
            pipeline.mDescriptorSet->mDescriptorSets.data(), // TODO don't just pass all of this in here
            0,
            nullptr
    );

    PushConstantsObject pushConstants{
            {mSwapchain->mExtent.width, mSwapchain->mExtent.height}
    };
    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.pushConstants(
            pipeline.mPipelineLayout,
            vk::ShaderStageFlagBits::eAll,
            0,
            sizeof(PushConstantsObject),
            &pushConstants
    );

    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.drawIndexed(
            mesh.indexPosition.count,
            1,
            mesh.indexPosition.memoryIndex / sizeof(uint32_t),
            static_cast<int32_t>(mesh.vertexPosition.memoryIndex / sizeof(uint32_t)),
            0
    );

    mCommandBuffers[*mCurrentSwapchainFramebuffer].mCommandBuffer.draw(
            mesh.vertexPosition.count,
            1,
            0,
            0
    );

    // TODO this->drawUi(ecs);
}

void VulkanAPI::recordUiDraw() {
    dnAssert(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");
    mGui->endFrame(mCommandBuffers[*mCurrentSwapchainFramebuffer]);
    // TODO maybe this is not the most ideal way to do this.
    mGui->beginFrame();
}

void VulkanAPI::drawFrame(double delta) {
    dnAssert(mCurrentSwapchainFramebuffer.has_value(), "Can not draw if no image has been acquired.");

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

    mContext->mGraphicsQueue.submit(submitInfo, mInFlightFence->mFence);

    vk::PresentInfoKHR presentInfo{
            static_cast<uint32_t>(signalSemaphores.size()),
            signalSemaphores.data(),
            1,
            &mSwapchain->mSwapchain,
            &(*mCurrentSwapchainFramebuffer),
            nullptr
    };
    auto result = mContext->mPresentQueue.presentKHR(presentInfo);

    require(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR, "An error has occured while rendering");

    mCurrentSwapchainFramebuffer.reset();
}

VulkanAPI::~VulkanAPI() {
    log::d("Destroying VulkanAPI");
    mInFlightFence->await();

    mGui.reset();

    mSampler.reset();
    mInFlightFence.reset();
    mImageAvailableSemaphore.reset();
    mRenderFinishedSemaphore.reset();

    mCommandBuffers.clear();
    mCommandPool.reset();
    mUniformBuffer.reset();
    mMeshes.reset();
    mTextures.reset();
    mPipelines.reset();
    mSwapchain.reset();
    mContext.reset();
}