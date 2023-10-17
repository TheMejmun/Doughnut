//
// Created by Saman on 18.04.23.
//

#include "graphics/renderer.h"
#include "graphics/uniform_buffer_object.h"
#include "graphics/vulkan/vulkan_swapchain.h"
#include "util/performance_logging.h"
#include "graphics/vulkan/vulkan_buffers.h"
#include "io/logger.h"

// SYSTEMS THAT PLUG INTO THE ECS

using namespace Doughnut;
using namespace Doughnut::GFX;

void Renderer::uploadRenderables(EntityManagerSpec &ecs) {
    auto entities = ecs.requestAll<RenderMesh>();
    for (auto renderMesh: entities) {
        if (renderMesh->isAllocated) continue;
        Vk::Buffers::uploadVertices(renderMesh->vertices);
        Vk::Buffers::uploadIndices(renderMesh->indices);
        renderMesh->isAllocated = true;
    }
}

void Renderer::uploadSimplifiedMeshes(EntityManagerSpec &ecs) {
    if (Vk::Buffers::waitingForFence) {
        Log::d("Waiting for fence");
        if (Vk::Buffers::isTransferQueueReady()) {
            Log::d("Ready");
            Vk::Buffers::finishTransfer();
        } else {
            Log::d("Not ready");
            return;
        }
    }
    const auto startTime = Timer::now();
    auto entities = ecs.requestAll<RenderMeshSimplifiable>();

    uint32_t bufferToUse = 1;
    if (Vk::Buffers::meshBufferToUse == 1) bufferToUse = 2;

    bool uploadedAny = false;

    for (auto renderMeshSimplifiable: entities) {
        if (renderMeshSimplifiable->updateSimplifiedMesh && renderMeshSimplifiable->simplifiedMeshMutex->try_lock()) {
            PerformanceLogging::meshUploadStarted();
            // TODO this upload produced a bad access error
            Vk::Buffers::uploadMesh(renderMeshSimplifiable->vertices, renderMeshSimplifiable->indices, true, bufferToUse);
            renderMeshSimplifiable->isAllocated = true;
            renderMeshSimplifiable->bufferIndex = bufferToUse;
            renderMeshSimplifiable->updateSimplifiedMesh = false;
            PerformanceLogging::meshUploadFinished({renderMeshSimplifiable->vertices.size(), renderMeshSimplifiable->indices.size() / 3});
            renderMeshSimplifiable->simplifiedMeshMutex->unlock();

            uploadedAny = true;
        }
    }

    if (uploadedAny) {
        // Treat this like a return
        auto &uiState = *ecs.requestAll<UiState>()[0];
        uiState.meshUploadTimeTaken = Timer::duration(startTime, Timer::now());
    }
}

void Renderer::destroyRenderables(EntityManagerSpec &ecs) {
    // TODO
}

void Renderer::updateUniformBuffer(const double &delta, EntityManagerSpec &ecs) {
    auto entities = ecs.requestAll<RenderMesh, Transformer4>();

    // TODO not just for one object
    UniformBufferObject ubo{};
    ubo.model = std::get<1>(entities)[0]->forward;

    auto cameras = ecs.requestAll<Projector, Transformer4>();
    Transformer4 *cameraTransform;
    Projector *cameraProjector;
    for (int i = 0; i < std::get<0>(cameras).size(); ++i) {
        if (std::get<0>(cameras)[i]->isMainCamera) {
            cameraTransform = std::get<1>(cameras)[i];
            cameraProjector = std::get<0>(cameras)[i];
            break;
        }
    }

    ubo.view = cameraProjector->getView(*cameraTransform);

    ubo.proj = cameraProjector->getProjection(Vk::Swapchain::aspectRatio);

    // TODO replace with push constants for small objects:
    // https://registry.khronos.org/vulkan/site/guide/latest/push_constants.html

    Vk::Buffers::nextUniformBuffer();
    memcpy(Vk::Buffers::getCurrentUniformBufferMapping(), &ubo, sizeof(ubo));
}