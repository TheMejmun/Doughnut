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
#include "imgui.h"

#include <stdexcept>
#include <vector>
#include <set>

using namespace dn;
using namespace dn::vulkan;

VulkanAPI::VulkanAPI(Window &window)
        : mContext(window, ContextConfiguration{}),
          mSwapchain(mContext, SwapchainConfiguration{false}),
          mCommandPipeline(mContext, CommandPipelineConfiguration{GRAPHICS, mSwapchain.mImageCount}),
          mImageAvailableSemaphore(mContext, SemaphoreConfiguration{}),
          mRenderFinishedSemaphore(mContext, SemaphoreConfiguration{}),
          mInFlightFence(mContext, FenceConfiguration{true}) {

    mMeshes.emplace(
            mContext
    );
    mTextures.emplace(
            mContext
    );

    // TODO one per frame in flight
    mUniformBuffer.emplace(
            mContext,
            BufferConfiguration{UNIFORM, true}
    );

    // if (!(*mSwapchain).shouldRecreate()) {
    // TODO do we need this condition?
    mPipelines.emplace(
            mContext,
            *mSwapchain.mRenderPass,
            PipelineCacheConfiguration{1u}
    );
    // }


    mSampler.emplace(
            mContext,
            SamplerConfiguration{
                    CLAMP
            }
    );

    mGui.emplace(
            mContext,
            window,
            *mSwapchain.mRenderPass,
            GuiConfiguration{
                    mSwapchain.mMinImageCount,
                    mSwapchain.mImageCount
            }
    );

    log::d("Created VulkanAPI");
}

bool VulkanAPI::nextImage() {
    mInFlightFence.await();

    if (mSwapchain.shouldRecreate()) {
        log::d("Requesting swapchain recreation");
        mSwapchain.recreate();
    }

    auto acquireImageResult = mSwapchain.acquireNextImage(mImageAvailableSemaphore);

    mInFlightFence.resetFence();

    if (!acquireImageResult.has_value()) {
        log::d("No swapchain image was acquired. Skipping frame.");
        mCurrentSwapchainFramebuffer.reset();
        return false;
    } else {
        // log::d("mCurrentSwapchainFramebuffer", acquireImageResult.value());
        mCurrentSwapchainFramebuffer = acquireImageResult;
        mCommandPipeline.nextBuffer();
        return true;
    }
}

void VulkanAPI::startRecording() {
    require_d(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");
    mCommandPipeline->startRecording();
}

void VulkanAPI::beginRenderPass() {
    require_d(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");

    std::array<vk::ClearValue, 2> clearValues{
            vk::ClearValue{{0.1f, 0.1f, 0.1f, 1.0f}},
            vk::ClearValue{{1.0f, 0}}
    };
    vk::RenderPassBeginInfo renderPassInfo{
            **mSwapchain.mRenderPass,
            *mSwapchain.getFramebuffer(*mCurrentSwapchainFramebuffer),
            vk::Rect2D{
                    {0, 0},
                    mSwapchain.mExtent
            },
            static_cast<uint32_t>(clearValues.size()),
            clearValues.data()
    };

    (**mCommandPipeline).beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void VulkanAPI::endRenderPass() {
    require_d(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");
    (**mCommandPipeline).endRenderPass();
}

void VulkanAPI::endRecording() {
    require_d(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");
    mCommandPipeline->endRecording();
}

void VulkanAPI::recordDraw(const Renderable &renderable) {
    require_d(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");

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
    (**mCommandPipeline).bindPipeline(vk::PipelineBindPoint::eGraphics,
                                      pipeline.mGraphicsPipeline);

    auto &mesh = mMeshes->get(renderable.model);

    // TODO set offsets
    // TODO do indices need to be offset by their in-buffer position?
    std::array<vk::Buffer, 1> vertexBuffers{mesh.vertexBuffer};
    std::array<vk::DeviceSize, 1> offsets{0};
    (**mCommandPipeline).bindVertexBuffers(
            0,
            1,
            vertexBuffers.data(),
            offsets.data()
    );

    (**mCommandPipeline).bindIndexBuffer(
            mesh.indexBuffer,
            0,
            vk::IndexType::eUint32
    );

    vk::Viewport viewport{
            0.0f,
            0.0f,
            static_cast<float>(mSwapchain.mExtent.width),
            static_cast<float>(mSwapchain.mExtent.height),
            0.0f,
            1.0f
    };

    (**mCommandPipeline).setViewport(
            0,
            1,
            &viewport
    );

    vk::Rect2D scissor{
            {0, 0},
            mSwapchain.mExtent
    };

    (**mCommandPipeline).setScissor(
            0,
            1,
            &scissor
    );

    (**mCommandPipeline).bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipeline.mPipelineLayout,
            0,
            1,
            pipeline.mDescriptorSet->mDescriptorSets.data(), // TODO don't just pass all of this in here
            0,
            nullptr
    );

    PushConstantsObject pushConstants{
            {mSwapchain.mExtent.width, mSwapchain.mExtent.height}
    };
    (**mCommandPipeline).pushConstants(
            pipeline.mPipelineLayout,
            vk::ShaderStageFlagBits::eAll,
            0,
            sizeof(PushConstantsObject),
            &pushConstants
    );

    (**mCommandPipeline).drawIndexed(
            mesh.indexPosition.count,
            1,
            mesh.indexPosition.memoryIndex / sizeof(uint32_t),
            static_cast<int32_t>(mesh.vertexPosition.memoryIndex / sizeof(uint32_t)),
            0
    );

    (**mCommandPipeline).draw(
            mesh.vertexPosition.count,
            1,
            0,
            0
    );
}

void VulkanAPI::recordUiDraw() {
    // TODO maybe this is not the most ideal way to do this.
    mGui->beginFrame();
    ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::ShowDemoWindow();
    require_d(mCurrentSwapchainFramebuffer.has_value(), "Can not record a command buffer if no image has been acquired.");
    mGui->endFrame(*mCommandPipeline);
}

void VulkanAPI::drawFrame(double delta) {
    require_d(mCurrentSwapchainFramebuffer.has_value(), "Can not draw if no image has been acquired.");

    // mInFlightFence->resetFence();

    // TODO updateUniformBuffer(delta, ecs);

    // or VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT

    std::array<vk::Semaphore, 1> waitSemaphores{*mImageAvailableSemaphore}; // index corresponding to wait stage
    std::array<vk::PipelineStageFlags, 1> waitStages{vk::PipelineStageFlagBits::eColorAttachmentOutput}; // Wait in fragment stage
    std::array<vk::Semaphore, 1> signalSemaphores{*mRenderFinishedSemaphore};
    vk::SubmitInfo submitInfo{
            static_cast<uint32_t>(waitSemaphores.size()),
            waitSemaphores.data(),
            waitStages.data(),
            1,
            &(**mCommandPipeline),
            static_cast<uint32_t>(signalSemaphores.size()),
            signalSemaphores.data(),
    };

    mContext.mGraphicsQueue.submit(submitInfo, *mInFlightFence);

    vk::PresentInfoKHR presentInfo{
            static_cast<uint32_t>(signalSemaphores.size()),
            signalSemaphores.data(),
            1,
            &mSwapchain.mSwapchain,
            &(*mCurrentSwapchainFramebuffer),
            nullptr
    };
    auto result = mContext.mPresentQueue.presentKHR(presentInfo);

    require(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR, "An error has occured while rendering");

    mCurrentSwapchainFramebuffer.reset();
}

VulkanAPI::~VulkanAPI() {
    log::d("Destroying VulkanAPI");

    mContext.awaitIdle();

    mGui.reset();

    mSampler.reset();

    mUniformBuffer.reset();
    mMeshes.reset();
    mTextures.reset();
    mPipelines.reset();
}