//
// Created by Sam on 2024-02-28.
//

#ifndef DOUGHNUTSANDBOX_GUI_H
#define DOUGHNUTSANDBOX_GUI_H

#include "graphics/vulkan/instance.h"
#include "descriptor_pool.h"
#include "render_pass.h"
#include "command_buffer.h"

namespace dn::vulkan {
    struct GuiConfiguration {
        uint32_t minImageCount;
        uint32_t imageCount;
    };

    class Gui {
    public:
        Gui(Instance &instance,
            Window &window,
            RenderPass &renderPass,
            const GuiConfiguration &config);

        ~Gui();

        void beginFrame();

        void endFrame(CommandBuffer &commandBuffer);

    private:
        Instance &mInstance;
        Window &mWindow;
        GuiConfiguration mConfig;

        DescriptorPool mDescriptorPool;
        vk::PipelineCache mPipelineCache;
    };
}

#endif //DOUGHNUTSANDBOX_GUI_H
