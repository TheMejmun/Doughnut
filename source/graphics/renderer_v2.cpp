//
// Created by Sam on 2024-01-23.
//

#include "graphics/renderer_v2.h"
#include "io/logger.h"

using namespace dn;

RendererV2::RendererV2(Window &window) : mAPI(window) {
    log::d("Creating RendererV2");
}

void RendererV2::drawFrame(double delta) {
    log::d("FRAME");

    mAPI.drawFrame(delta);
};

RendererV2::~RendererV2() {
    log::d("Destroying RendererV2");
}