//
// Created by Saman on 26.08.23.
//

#ifndef DOUGHNUT_RENDER_STATE_H
#define DOUGHNUT_RENDER_STATE_H

#include "preprocessor.h"
#include "graphics/v1/vulkan/vulkan_state.h"
#include "util/timer.h"

#include "GLFW/glfw3.h"
#include <string>

namespace dn {
    struct RenderState {
        std::string title{};

        GLFWwindow *window = nullptr;

        vulkan::State vulkanState{};
    };
};
#endif //DOUGHNUT_RENDER_STATE_H
