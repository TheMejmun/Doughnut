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
    createSwapchain();
}

VulkanAPI::~VulkanAPI() {
    log::d("Destroying VulkanAPI");
    destroySwapchain();
    mInstance.reset();
}

void VulkanAPI::createSwapchain() {
    mSwapchain.emplace(
            mInstance.value(),
            SwapchainConfiguration{false}
    );
}

void VulkanAPI::destroySwapchain() {
    mSwapchain.reset();
}