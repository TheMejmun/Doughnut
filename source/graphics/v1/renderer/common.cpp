//
// Created by Sam on 2023-04-11.
//

#include "graphics/v1/renderer.h"
#include "graphics/v1/vulkan/vulkan_instance.h"
#include "graphics/v1/vulkan/vulkan_swapchain.h"
#include "graphics/v1/vulkan/vulkan_images.h"
#include "graphics/v1/vulkan/vulkan_imgui.h"
#include "graphics/v1/vulkan/vulkan_buffers.h"
#include "io/logger.h"

using namespace dn;
using namespace dn;

Renderer::Renderer(const std::string &title, GLFWwindow *window) {
   log::d("Creating Renderer");

    // Reset
    this->state = {};
    vulkan::Buffers::meshBufferToUse = 0;

    this->state.title = title;
    this->state.window = window;

    this->initVulkan();
    vulkan::Imgui::create(this->state);
}

void Renderer::initVulkan() {
    vulkan::Instance::create(this->state.title);
    vulkan::Swapchain::createSurface(this->state.window);
    vulkan::Devices::create();
    vulkan::Swapchain::createSwapchain();
    createDescriptorSetLayout();
    createGraphicsPipeline();
    vulkan::Buffers::create();
    createDescriptorPool();
    createDescriptorSets();
    createCommandPool();
    vulkan::Images::createTextureImage();
    createSyncObjects();
}

void Renderer::destroyVulkan() {
    vkDestroySemaphore(vulkan::Devices::logical, this->imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(vulkan::Devices::logical, this->renderFinishedSemaphore, nullptr);
    vkDestroyFence(vulkan::Devices::logical, this->inFlightFence, nullptr);

    vulkan::Buffers::destroy();

    // vulkan::Buffers::destroyCommandBuffer(this->commandPool);
    vkDestroyCommandPool(vulkan::Devices::logical, this->commandPool, nullptr);
    vkDestroyDescriptorPool(vulkan::Devices::logical, this->descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(vulkan::Devices::logical, this->descriptorSetLayout, nullptr);
    vkDestroyPipeline(vulkan::Devices::logical, this->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(vulkan::Devices::logical, this->pipelineLayout, nullptr);
    vulkan::Swapchain::destroySwapchain();
    vulkan::Devices::destroy();
    vkDestroySurfaceKHR(vulkan::Instance::instance, vulkan::Swapchain::surface, nullptr);

    vulkan::Instance::destroy();
}

Renderer::~Renderer() {
   log::d("Destroying Renderer");

    if (this->simplifiedMeshAllocationThread.joinable())
        this->simplifiedMeshAllocationThread.join();

    // Wait until resources are not actively being used anymore
    vkDeviceWaitIdle(vulkan::Devices::logical);

    vulkan::Imgui::destroy();

    this->destroyVulkan();
}