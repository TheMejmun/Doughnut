//
// Created by Saman on 26.08.23.
//


#include "graphics/renderer.h"
#include "graphics/vulkan/vulkan_imgui.h"
#include "graphics/vulkan/vulkan_buffers.h"

using namespace Doughnut::GFX;

UiState *Renderer::getUiState() {
    return &this->state.uiState;
}

void Renderer::resetMesh() {
    Vk::Buffers::resetMeshBufferToUse();
}

void Renderer::drawUi(){
    this->state.uiState.currentMeshVertices = Vk::Buffers::vertexCount[Vk::Buffers::meshBufferToUse];
    this->state.uiState.currentMeshTriangles = Vk::Buffers::indexCount[Vk::Buffers::meshBufferToUse] / 3;
    Vk::Imgui::draw(this->state);
}