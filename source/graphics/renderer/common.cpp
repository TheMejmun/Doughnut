//
// Created by Sam on 2023-04-11.
//

#include "graphics/renderer.h"
#include "graphics/vulkan/vulkan_instance.h"
#include "graphics/vulkan/vulkan_swapchain.h"
#include "graphics/vulkan/vulkan_images.h"
#include "graphics/vulkan/vulkan_imgui.h"
#include "graphics/vulkan/vulkan_buffers.h"

using namespace Doughnut::GFX;

Renderer::Renderer(const std::string &title, GLFWwindow *window) {
    info( "Creating Renderer" );

    // Reset
    this->state = {};
    Vk::Buffers::meshBufferToUse = 0;

    this->state.title = title;
    this->state.window = window;

    this->state.uiState.title = title;
    this->state.uiState.window = window;

    this->initVulkan();
    Vk::Imgui::create(this->state);
}

void Renderer::initVulkan() {
    Vk::Instance::create(this->state.title);
    Vk::Swapchain::createSurface(this->state.window);
    Vk::Devices::create();
    Vk::Swapchain::createSwapchain();
    createDescriptorSetLayout();
    createGraphicsPipeline();
    Vk::Buffers::create();
    createDescriptorPool();
    createDescriptorSets();
    createCommandPool();
    Vk::Images::createTextureImage();
    createSyncObjects();
}

void Renderer::destroyVulkan() {
    vkDestroySemaphore(Vk::Devices::logical, this->imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(Vk::Devices::logical, this->renderFinishedSemaphore, nullptr);
    vkDestroyFence(Vk::Devices::logical, this->inFlightFence, nullptr);

    Vk::Buffers::destroy();

    // Vk::Buffers::destroyCommandBuffer(this->commandPool);
    vkDestroyCommandPool(Vk::Devices::logical, this->commandPool, nullptr);
    vkDestroyDescriptorPool(Vk::Devices::logical, this->descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(Vk::Devices::logical, this->descriptorSetLayout, nullptr);
    vkDestroyPipeline(Vk::Devices::logical, this->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(Vk::Devices::logical, this->pipelineLayout, nullptr);
    Vk::Swapchain::destroySwapchain();
    Vk::Devices::destroy();
    vkDestroySurfaceKHR(Vk::Instance::instance, Vk::Swapchain::surface, nullptr);

    Vk::Instance::destroy();
}

Renderer::~Renderer() {
    info( "Destroying Renderer" );

    if (this->simplifiedMeshAllocationThread.joinable())
        this->simplifiedMeshAllocationThread.join();

    // Wait until resources are not actively being used anymore
    vkDeviceWaitIdle(Vk::Devices::logical);

    Vk::Imgui::destroy();

    this->destroyVulkan();
}