//
// Created by Sam on 2024-01-23.
//

#include "graphics/renderer_v2.h"
#include "io/logger.h"
#include "graphics/triangle.h"

using namespace dn;

RendererV2::RendererV2(Window &window) : mAPI(window) {
    log::d("Creating RendererV2");

    // TODO upload a triangle
    Triangle triangle{};
    indexPosition = mAPI.mIndexBuffer->queueUpload(triangle.mesh.indices);
    mAPI.mIndexBuffer->awaitUpload();
    vertexPosition = mAPI.mVertexBuffer->queueUpload(triangle.mesh.vertices);
    mAPI.mVertexBuffer->awaitUpload();
}

void RendererV2::drawFrame(double delta) {
    log::d("FRAME");

    mAPI.nextImage();
    mAPI.startRecording();
    mAPI.beginRenderPass();
    // TODO record triangle draw
    mAPI.recordMeshDraw(
            vertexPosition.position,
            indexPosition.position
    );
    mAPI.endRenderPass();
    mAPI.endRecording();
    // TODO submit somehow?
    mAPI.drawFrame(delta);
};

RendererV2::~RendererV2() {
    log::d("Destroying RendererV2");
}