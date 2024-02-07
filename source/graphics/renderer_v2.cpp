//
// Created by Sam on 2024-01-23.
//

#include "graphics/renderer_v2.h"
#include "io/logger.h"

using namespace dn;

RendererV2::RendererV2(Window *window, const std::string &title) : mAPI(window, title) {
   log::d("Creating RendererV2");
}

RendererV2::~RendererV2() {
   log::d("Destroying RendererV2");
}