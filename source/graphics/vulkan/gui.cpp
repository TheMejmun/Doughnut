//
// Created by Sam on 2024-02-28.
//

#include "graphics/vulkan/gui.h"
#include "io/logger.h"

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

using namespace dn;
using namespace dn::vulkan;

void checkVkResult(VkResult err) {
    if (err == 0) return;

    if (err < 0) {
        std::stringstream sstream;
        sstream << "Failed to create  GUI. VkResult: " << err;
        throw std::runtime_error(sstream.str());
    }
}

Gui::Gui(Instance &instance,
         Window &window,
         RenderPass &renderPass,
         const GuiConfiguration &config)
        : mInstance(instance),
          mWindow(window),
          mConfig(config),
          mDescriptorPool(
                  mInstance,
                  DescriptorPoolConfiguration{
                          2u,
                          {{SAMPLER, 1}},
                          true
                  }
          ) {
    log::d("Creating GUI");

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.WantCaptureMouse = true;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform / Renderer backends
    ImGui_ImplGlfw_InitForVulkan(mWindow.mGlfwWindow, true);
    ImGui_ImplVulkan_InitInfo initInfo{
//            VkInstance                      Instance;
//            VkPhysicalDevice                PhysicalDevice;
//            VkDevice                        Device;
//            uint32_t                        QueueFamily;
//            VkQueue                         Queue;
//            VkPipelineCache                 PipelineCache;
//            VkDescriptorPool                DescriptorPool;
//            uint32_t                        Subpass;
//            uint32_t                        MinImageCount;          // >= 2
//            uint32_t                        ImageCount;             // >= MinImageCount
//            VkSampleCountFlagBits           MSAASamples;            // >= VK_SAMPLE_COUNT_1_BIT (0 -> default to VK_SAMPLE_COUNT_1_BIT)
//
//            // Dynamic Rendering (Optional)
//            bool                            UseDynamicRendering;    // Need to explicitly enable VK_KHR_dynamic_rendering extension to use this, even for Vulkan 1.3.
//            VkFormat                        ColorAttachmentFormat;  // Required for dynamic rendering
//
//            // Allocation, Debugging
//            const VkAllocationCallbacks*    Allocator;
//            void                            (*CheckVkResultFn)(VkResult err);
            mInstance.mInstance,
            mInstance.mPhysicalDevice,
            mInstance.mDevice,
            *mInstance.mQueueFamilyIndices.graphicsFamily,
            mInstance.mGraphicsQueue,
            mPipelineCache, // TODO
            mDescriptorPool.mDescriptorPool,
            0, // TODO
            config.minImageCount,
            config.imageCount,
            VK_SAMPLE_COUNT_1_BIT,
            false,
            {},
            nullptr,
            checkVkResult
    };
    // init_info.RenderPass = wd->RenderPass;
    // init_info.Allocator = g_Allocator;
    ImGui_ImplVulkan_Init(&initInfo, renderPass.mRenderPass);

// Load Fonts
// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
// - Read 'docs/FONTS.md' for more instructions and details.
// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
//io.Fonts->AddFontDefault();
//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
//IM_ASSERT(font != nullptr);

    // Fonts:
//    VkCommandPool command_pool = Buffers::transferCommandPool;
//    VkCommandBuffer command_buffer = Buffers::transferCommandBuffer;
//
//    checkVkResult(vkResetCommandPool(Devices::logical, command_pool, 0));
//    VkCommandBufferBeginInfo beginInfo = {};
//    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//    beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//    checkVkResult(vkBeginCommandBuffer(command_buffer, &beginInfo));
//
//    // TODO find out why this function needs VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT and provide the appropriate queue
//    // TODO why does this not take the commandBuffer anymore. Probably because docking branch is used now.
//    ImGui_ImplVulkan_CreateFontsTexture();
//
//    VkSubmitInfo endInfo = {};
//    endInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//    endInfo.commandBufferCount = 1;
//    endInfo.pCommandBuffers = &command_buffer;
//    checkVkResult(vkEndCommandBuffer(command_buffer));
//    // TODO arbitrarily picked this queue because it was global
//    checkVkResult(vkQueueSubmit(Buffers::transferQueue, 1, &endInfo, VK_NULL_HANDLE));
//
//    checkVkResult(vkDeviceWaitIdle(Devices::logical));
//    // TODO why did this change. Probably because docking branch is used now.
////    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Gui::recordDraw(CommandBuffer& commandBuffer) {
    auto size = mWindow.getSize();
    ImVec2 scale =  {size.scale, size.scale};
    ImGui::GetIO().DisplaySize = {static_cast<float>(size.windowWidth),
                                  static_cast<float>(size.windowHeight)};
    ImGui::GetIO().DisplayFramebufferScale =scale;

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

    draw_data->FramebufferScale = scale;
    ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer.mCommandBuffer);
}

Gui::~Gui() {
    log::d("Destroying GUI");

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}