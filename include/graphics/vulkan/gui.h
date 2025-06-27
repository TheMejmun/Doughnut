//
// Created by Sam on 2024-02-28.
//

#ifndef DOUGHNUT_GUI_H
#define DOUGHNUT_GUI_H

#include "graphics/vulkan/context.h"
#include "graphics/vulkan/handles/descriptor_pool.h"
#include "graphics/vulkan/handles/render_pass.h"
#include "graphics/vulkan/handles/command_buffer.h"

namespace dn::vulkan {
    struct GuiConfiguration {
        uint32_t minImageCount;
        uint32_t imageCount;
    };

    class Gui : WindowEventListener {
    public:
        Gui(Context &context,
            Window &window,
            RenderPass &renderPass,
            const GuiConfiguration &config);

        ~Gui();

        void beginFrame();

        void endFrame(CommandBuffer &commandBuffer);

        void onWindowEvent(const SDL_Event &event) override;

    private:
        Context &mContext;
        Window &mWindow;
        GuiConfiguration mConfig;

        DescriptorPool mDescriptorPool;
        vk::PipelineCache mPipelineCache;
    };
}

#endif //DOUGHNUT_GUI_H
