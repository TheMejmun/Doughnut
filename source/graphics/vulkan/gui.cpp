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

Gui::Gui(Context &context,
         Window &window,
         RenderPass &renderPass,
         const GuiConfiguration &config)
        : mContext(context),
          mWindow(window),
          mConfig(config),
          mDescriptorPool(
                  mContext,
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
            mContext.mInstance,
            mContext.mPhysicalDevice,
            mContext.mDevice,
            *mContext.mQueueFamilyIndices.graphicsFamily,
            mContext.mGraphicsQueue,
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
}

// TODO consider not calculating size twice per frame
void Gui::beginFrame() {
    auto size = mWindow.getSize();
    ImVec2 scale = {size.scale, size.scale};
    ImGui::GetIO().DisplaySize = {static_cast<float>(size.windowWidth),
                                  static_cast<float>(size.windowHeight)};
    ImGui::GetIO().DisplayFramebufferScale = scale;

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::endFrame(CommandBuffer &commandBuffer) {
    // ImGui::EndFrame(); // Automatically called by Render()
    // ImGui::UpdatePlatformWindows(); // For Viewports

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool isMinimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

    if (!isMinimized) {
        auto size = mWindow.getSize();
        ImVec2 scale = {size.scale, size.scale};
        draw_data->FramebufferScale = scale;
        ImGui_ImplVulkan_RenderDrawData(draw_data, *commandBuffer);
    }
}

Gui::~Gui() {
    log::d("Destroying GUI");

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}