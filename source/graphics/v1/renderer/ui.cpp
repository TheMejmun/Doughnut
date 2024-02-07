//
// Created by Saman on 26.08.23.
//


#include "graphics/v1/renderer.h"
#include "graphics/v1/vulkan/vulkan_imgui.h"
#include "graphics/v1/vulkan/vulkan_buffers.h"

using namespace dn;

void Renderer::resetMesh() {
    vulkan::Buffers::resetMeshBufferToUse();
}

void Renderer::drawUi(EntityManagerSpec &ecs) {
    auto &uiState = *ecs.template getArchetype<UiState>()[0].components;
    uiState.currentMeshVertices = vulkan::Buffers::vertexCount[vulkan::Buffers::meshBufferToUse];
    uiState.currentMeshTriangles = vulkan::Buffers::indexCount[vulkan::Buffers::meshBufferToUse] / 3;
    vulkan::Imgui::draw(this->state, uiState);
}