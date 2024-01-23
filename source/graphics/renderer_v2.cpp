//
// Created by Sam on 2024-01-23.
//

#include "graphics/renderer_v2.h"
#include "io/logger.h"

using namespace Doughnut;

RendererV2::RendererV2(GLFWwindow *window, const std::string &title) : mAPI(window, title) {
    Log::i("Creating RendererV2");
}

RendererV2::~RendererV2() {
    Log::i("Destroying RendererV2");
}