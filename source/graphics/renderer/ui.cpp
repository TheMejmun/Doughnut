//
// Created by Saman on 26.08.23.
//


#include "graphics/renderer.h"
#include "graphics/vulkan/vulkan_imgui.h"
#include "graphics/vulkan/vulkan_buffers.h"

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