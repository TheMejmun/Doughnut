//
// Created by Saman on 26.08.23.
//


#include "graphics/v1/renderer.h"
#include "graphics/v1/vulkan/vulkan_imgui.h"
#include "graphics/v1/vulkan/vulkan_buffers.h"

using namespace Doughnut::Graphics;

void Renderer::resetMesh() {
    Vk::Buffers::resetMeshBufferToUse();
}

void Renderer::drawUi(EntityManagerSpec &ecs) {
    auto &uiState = *ecs.template getArchetype<UiState>()[0].components;
    uiState.currentMeshVertices = Vk::Buffers::vertexCount[Vk::Buffers::meshBufferToUse];
    uiState.currentMeshTriangles = Vk::Buffers::indexCount[Vk::Buffers::meshBufferToUse] / 3;
    Vk::Imgui::draw(this->state, uiState);
}