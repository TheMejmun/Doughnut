//
// Created by Saman on 26.08.23.
//

#ifndef DOUGHNUT_VULKAN_IMGUI_H
#define DOUGHNUT_VULKAN_IMGUI_H

#include "preprocessor.h"
#include "ecs/components/ui_state.h"
#include "graphics/v1/render_state.h"

namespace dn::vulkan::Imgui {
    void create(RenderState &state);

    void draw(RenderState &renderState, UiState &uiState);

    void recalculateScale(RenderState &state);

    void destroy();
}

#endif //DOUGHNUT_VULKAN_IMGUI_H
