//
// Created by Saman on 26.08.23.
//


#include "graphics/v1/vulkan/vulkan_imgui.h"
#include "graphics/v1/vulkan/vulkan_instance.h"
#include "graphics/v1/vulkan/vulkan_devices.h"
#include "graphics/v1/vulkan/vulkan_swapchain.h"
#include "graphics/v1/vulkan/vulkan_renderpasses.h"
#include "graphics/v1/vulkan/vulkan_buffers.h"
#include "graphics/v1/ui.h"
#include "io/logger.h"

#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"
#include <sstream>

using namespace Doughnut;
using namespace Doughnut::Graphics::Vk;

VkDescriptorPool uiDescriptorPool;
float scale = 1.0f;
ImVec2 scaleVec2 = {scale, scale};

static void checkVkResult(VkResult err) {
    if (err == 0) return;

    if (err < 0) {
        std::stringstream sstream;
        sstream << "Failed to create  GUI. VkResult: " << err;
        throw std::runtime_error(sstream.str());
    }
}

void Imgui::create(RenderState &state) {
    Log::i("Creating Imgui");

    // https://github.com/ocornut/imgui/blob/master/examples/example_glfw_vulkan/main.cpp

    // Descriptor pool:
    VkDescriptorPoolSize pool_sizes[] = {
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
    };
    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1;
    poolInfo.poolSizeCount = (uint32_t) IM_ARRAYSIZE(pool_sizes);
    poolInfo.pPoolSizes = pool_sizes;
    checkVkResult(
            vkCreateDescriptorPool(Devices::logical, &poolInfo, nullptr, &uiDescriptorPool)
    );

    // Context:
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.WantCaptureMouse = true;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Style:
    ImGui::StyleColorsDark();

    // Renderer
    ImGui_ImplGlfw_InitForVulkan(state.window, true);
    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = Instance::instance;
    initInfo.PhysicalDevice = Devices::physical;
    initInfo.Device = Devices::logical;
    initInfo.QueueFamily = Devices::queueFamilyIndices.graphicsFamily.value();
    initInfo.Queue = Devices::graphicsQueue;
    initInfo.DescriptorPool = uiDescriptorPool; // TODO
    initInfo.Subpass = 0;
    initInfo.MinImageCount = Swapchain::minImageCount;
    initInfo.ImageCount = Swapchain::imageCount;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.CheckVkResultFn = checkVkResult;
    ImGui_ImplVulkan_Init(&initInfo, RenderPasses::renderPass);

    // Fonts:
    VkCommandPool command_pool = Buffers::transferCommandPool;
    VkCommandBuffer command_buffer = Buffers::transferCommandBuffer;

    checkVkResult(vkResetCommandPool(Devices::logical, command_pool, 0));
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    checkVkResult(vkBeginCommandBuffer(command_buffer, &beginInfo));

    // TODO find out why this function needs VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT and provide the appropriate queue
    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

    VkSubmitInfo endInfo = {};
    endInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    endInfo.commandBufferCount = 1;
    endInfo.pCommandBuffers = &command_buffer;
    checkVkResult(vkEndCommandBuffer(command_buffer));
    // TODO arbitrarily picked this queue because it was global
    checkVkResult(vkQueueSubmit(Buffers::transferQueue, 1, &endInfo, VK_NULL_HANDLE));

    checkVkResult(vkDeviceWaitIdle(Devices::logical));
    ImGui_ImplVulkan_DestroyFontUploadObjects();

    Imgui::recalculateScale(state);
}

void Imgui::recalculateScale(RenderState &state) {
    int fbWidth, fbHeight, wWidth, wHeight;
    glfwGetFramebufferSize(state.window, &fbWidth, &fbHeight);
    glfwGetWindowSize(state.window, &wWidth, &wHeight);
    // printf("Window width: %d,\tframebuffer width: %d\n", wWidth, fbWidth);
    // printf("Window height: %d,\tframebuffer height: %d\n", wHeight, fbHeight);

    scale = (static_cast<float>(fbWidth) / static_cast<float>(wWidth));
    scaleVec2 = {scale, scale};
}

void Imgui::draw(RenderState &renderState, UiState &uiState) {
    int width, height;
    glfwGetFramebufferSize(renderState.window, &width, &height);
    ImGui::GetIO().DisplaySize = {static_cast<float>(Swapchain::framebufferWidth),
                                  static_cast<float>(Swapchain::framebufferHeight)};
    ImGui::GetIO().DisplaySize = {static_cast<float>(width),
                                  static_cast<float>(height)};
    ImGui::GetIO().DisplayFramebufferScale = scaleVec2;

//    ImGui_ImplVulkan_NewFrame(); Why was this here twice?
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();

    UI::update(uiState);

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    draw_data->FramebufferScale = scaleVec2;
    ImGui_ImplVulkan_RenderDrawData(draw_data, Buffers::commandBuffer);
}

void Imgui::destroy() {
    Log::i("Destroying Imgui");

    vkDeviceWaitIdle(Devices::logical);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    vkDestroyDescriptorPool(Devices::logical, uiDescriptorPool, nullptr);
}