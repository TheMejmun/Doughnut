//
// Created by Sam on 2023-04-08.
//

#include "application.h"
#include "io/logger.h"
#include "ecs/entities/dense_sphere.h"
#include "ecs/entities/monkey.h"
#include "ecs/entities/camera.h"
#include "ecs/systems/camera_controller.h"
#include "util/performance_logging.h"
#include "ecs/systems/mesh_simplifier_controller.h"
#include "ecs/systems/sphere_controller.h"
#include "ecs/entities/ui_state_entity.h"

#include <iomanip>

using namespace dn;

void Application::run() {
    try {

        do {
            mExitAfterMainLoop = true;
            init();
            mainLoop();
            destroy();
        } while (!mExitAfterMainLoop);

    } catch (const std::exception &e) {
        dn::log::flush();
        throw e;
        std::cerr << e.what() << std::endl;
    }
}

void Application::init() {
   log::d("Creating Application");

#ifdef NDEBUG
   log::i(this->mTitle, "is running in release mode.");
#else
   log::i(this->mTitle, "is running in debug mode.");
#endif

    mESM = std::make_unique<EntitySystemManagerSpec>();
    mWindowManager = std::make_unique<Window>(this->mTitle);
    mInputManager = std::make_unique<InputController>(mWindowManager->glfwWindow);
    mRenderer = std::make_unique<Renderer>(this->mTitle, mWindowManager->glfwWindow);

    // Entities
    Camera::upload(mESM->mEntities);
    mESM->mEntities.getArchetype<Projector>()[0].components->isMainCamera = true;

    InputStateEntity::upload(mESM->mEntities);
    UiStateEntity::upload(mESM->mEntities);
    const auto uiState = mESM->mEntities.template getArchetype<UiState>()[0].components;
    uiState->isMonkeyMesh = mMonkeyMode;
    uiState->title = mTitle;

    if (mMonkeyMode) {
        Monkey::upload(mESM->mEntities);
    } else {
        DenseSphere::upload(mESM->mEntities);
    }

    // Systems
    mESM->mSystems.insertSystem<CameraController, 0>();
    mESM->mSystems.insertSystem<MeshSimplifierController, 0>();
    mESM->mSystems.insertSystem<SphereController, 0>();
}

void Application::mainLoop() {
    while (!mWindowManager->shouldClose()) {

        mESM->mEntities.commitDeletions();

        // Input
        mInputManager->update(mDeltaTime, mESM->mEntities);
        const auto inputState = mESM->mEntities.template getArchetype<InputState>()[0].components;
        if (inputState->closeWindow == IM_DOWN_EVENT)
            mWindowManager->close();
        if (inputState->toggleFullscreen == IM_DOWN_EVENT)
            mWindowManager->toggleFullscreen();

        // UI
        const auto uiState = mESM->mEntities.template getArchetype<UiState>()[0].components;
        uiState->fps.update(mDeltaTime);
        uiState->cpuWaitTime = mCurrentCpuWaitTime;

        if (uiState->switchMesh) {
            mExitAfterMainLoop = false;
            mMonkeyMode = !mMonkeyMode;
            mWindowManager->close();
        }

        // Update camera Z for UI
        auto cameras = mESM->mEntities.template getArchetype<Projector, Transformer4>();
        for (auto &camera: cameras) {
            if (camera.get<Projector>()->isMainCamera) {
                uiState->cameraZ = camera.get<Transformer4>()->getPosition().z;
                break;
            }
        }

        mESM->mSystems.update(mDeltaTime);

        // Render
        if (uiState->returnToOriginalMeshBuffer)
            mRenderer->resetMesh();
        mCurrentCpuWaitTime = mRenderer->draw(mDeltaTime, mESM->mEntities);

        // Benchmark
        auto time = now();
        mDeltaTime = duration(mLastTimestamp, time);
        mLastTimestamp = time;

        // Performance logging
        PerformanceLogging::update(uiState);
        PerformanceLogging::newFrame({
                                             .cpuWaitTime = mCurrentCpuWaitTime,
                                             .totalFrameTime = mDeltaTime
                                     });
    }
}

void Application::destroy() {
   log::d("Destroying Application");

    // Reset in order
    mRenderer.reset();
    mInputManager.reset();
    mWindowManager.reset();
    mESM.reset();
}
