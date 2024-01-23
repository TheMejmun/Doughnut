//
// Created by Saman on 18.04.23.
//

#include "graphics/v1/renderer.h"
#include "graphics/v1/uniform_buffer_object.h"
#include "graphics/v1/vulkan/vulkan_swapchain.h"
#include "util/performance_logging.h"
#include "graphics/v1/vulkan/vulkan_buffers.h"
#include "io/logger.h"

// SYSTEMS THAT PLUG INTO THE ECS

using namespace Doughnut;
using namespace Doughnut::Graphics;

void Renderer::uploadRenderables(EntityManagerSpec &ecs) {
    const auto entities = ecs.getArchetype<RenderMesh>();
    for (auto entity: entities) {
        auto renderMesh = entity.components;
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
    auto entities = ecs.getArchetype<RenderMeshSimplifiable>();

    uint32_t bufferToUse = 1;
    if (Vk::Buffers::meshBufferToUse == 1) bufferToUse = 2;

    bool uploadedAny = false;

    for (const auto &entity: entities) {
        const auto renderMeshSimplifiable = entity.components;
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
        auto &uiState = *ecs.getArchetype<UiState>()[0].components;
        uiState.meshUploadTimeTaken = Timer::duration(startTime, Timer::now());
    }
}

void Renderer::destroyRenderables(EntityManagerSpec &ecs) {
    // TODO
}

void Renderer::updateUniformBuffer(const double &delta, EntityManagerSpec &ecs) {
    const auto entities = ecs.getArchetype<RenderMesh, Transformer4>();

    // TODO not just for one object
    UniformBufferObject ubo{};
    const auto &transformer = entities[0].get<Transformer4>();
    ubo.model = transformer->forward;

    const auto cameras = ecs.getArchetype<Projector, Transformer4>();
    Transformer4 *cameraTransform;
    Projector *cameraProjector;
    for (auto &camera: cameras) {
        if (camera.get<Projector>()->isMainCamera) {
            cameraTransform = camera.get<Transformer4>();
            cameraProjector = camera.get<Projector>();
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