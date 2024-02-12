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

    if (!(*mSwapchain).shouldRecreate()) {
        // TODO do we need this condition?
        mPipeline.emplace(
                *mInstance,
                *mSwapchain->mRenderPass,
                *mUniformBuffer,
                PipelineConfiguration{}
        );
    }

    mCommandPool.emplace(
            *mInstance
    );
    mCommandBuffer.emplace(
            *mInstance,
            *mCommandPool
    );

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

void VulkanAPI::drawFrame(double delta) {
    if (mSwapchain->shouldRecreate()) {
        bool success = mSwapchain->recreate();
        if (success) {
            log::d("Created new swapchain");
            // TODO vulkan::Swapchain::needsNewSwapchain = false;
        } else {
            log::d("Failed to create new swapchain");
            return;
        }
    }

//    uploadRenderables(ecs);
//    uploadSimplifiedMeshes(ecs);
//    destroyRenderables(ecs);

    mInFlightFence->await();

    auto acquireImageResult = mSwapchain->acquireNextImage(*mImageAvailableSemaphore);
    if (!acquireImageResult.has_value()) {
        return;
    }

    // TODO WIP -------------------------------------------------------------------------------------------------

    if (acquireImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
        log::d("Swapchain is out of date");
        vulkan::Swapchain::recreateSwapchain(this->state);
        return duration(beforeFence, afterFence); // Why not
    } else if (acquireImageResult == VK_SUBOPTIMAL_KHR) {
        log::d("Swapchain is suboptimal");
        vulkan::Swapchain::needsNewSwapchain = true;

    } else if (acquireImageResult != VK_SUCCESS) {
        throw std::runtime_error("Failed to acquire swapchain image!");
    }

    // Avoid deadlock if recreating -> move to after success check
    vkResetFences(vulkan::Devices::logical, 1, &this->inFlightFence);

    auto commandBuffer = vulkan::Buffers::commandBuffer;

    updateUniformBuffer(delta, ecs);

    vkResetCommandBuffer(commandBuffer, 0); // I am not convinced this is necessary
    recordCommandBuffer(ecs, commandBuffer, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {this->imageAvailableSemaphore}; // index corresponding to wait stage
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}; // Wait in fragment stage
    // or VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = {this->renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vulkan::Devices::graphicsQueue, 1, &submitInfo, this->inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapchains[] = {vulkan::Swapchain::swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Per swapchain acquireImageResult

    vkQueuePresentKHR(vulkan::Devices::presentQueue, &presentInfo);
}

VulkanAPI::~VulkanAPI() {
    log::d("Destroying VulkanAPI");
    mInFlightFence.reset();
    mImageAvailableSemaphore.reset();
    mRenderFinishedSemaphore.reset();

    mCommandBuffer.reset();
    mCommandPool.reset();
    mUniformBuffer.reset();
    mVertexBuffer.reset();
    mIndexBuffer.reset();
    mPipeline.reset();
    mSwapchain.reset();
    mInstance.reset();
}