//
// Created by Saman on 26.08.23.
//

#ifndef REALTIME_CELL_COLLAPSE_VULKAN_IMGUI_H
#define REALTIME_CELL_COLLAPSE_VULKAN_IMGUI_H

#include "preprocessor.h"
#include "ecs/components/ui_state.h"
#include "graphics/render_state.h"

namespace Doughnut::Graphics::Vk::Imgui {
    void create(RenderState &state);

    void draw(RenderState &renderState, UiState &uiState);

    void recalculateScale(RenderState &state);

    void destroy();
}

#endif //REALTIME_CELL_COLLAPSE_VULKAN_IMGUI_H
