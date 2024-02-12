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
        mPipeline.emplace(
                *mInstance,
                *mSwapchain->mRenderPass,
                *mUniformBuffer,
                PipelineConfiguration{}
        );
    }
}

VulkanAPI::~VulkanAPI() {
    log::d("Destroying VulkanAPI");
    mUniformBuffer.reset();
    mVertexBuffer.reset();
    mIndexBuffer.reset();
    mPipeline.reset();
    mSwapchain.reset();
    mInstance.reset();
}