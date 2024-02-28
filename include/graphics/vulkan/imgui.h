//
// Created by Sam on 2024-02-28.
//

#ifndef DOUGHNUTSANDBOX_IMGUI_H
#define DOUGHNUTSANDBOX_IMGUI_H

#include "graphics/vulkan/instance.h"
#include "descriptor_pool.h"
#include "render_pass.h"
#include "command_buffer.h"

namespace dn::vulkan {
    struct GUIConfiguration {
        uint32_t minImageCount;
        uint32_t imageCount;
    };

    class GUI {
    public:
        GUI(Instance &instance,
            Window &window,
            RenderPass &renderPass,
            const GUIConfiguration &config);

        ~GUI();

        void recordDraw(CommandBuffer& commandBuffer);

    private:
        Instance &mInstance;
        Window &mWindow;
        GUIConfiguration mConfig;

        DescriptorPool mDescriptorPool;
        vk::PipelineCache mPipelineCache;
    };
}

#endif //DOUGHNUTSANDBOX_IMGUI_H
