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

using namespace Doughnut;

void Application::run() {
    do {
        mExitAfterMainLoop = true;
        init();
        mainLoop();
        destroy();
    } while (!mExitAfterMainLoop);
}

void Application::init() {
    Log::i("Creating Application");

#ifdef NDEBUG
    Log::i(this->mTitle, "is running in release mode.");
#else
    Log::i(this->mTitle, "is running in debug mode.");
#endif

    mESM = std::make_unique<EntitySystemManagerSpec>();
    mWindowManager = std::make_unique<WindowManager>(this->mTitle);
    mInputManager = std::make_unique<InputController>(mWindowManager->window);
    mRenderer = std::make_unique<GFX::Renderer>(this->mTitle, mWindowManager->window);

    // Entities
    Camera::upload(mESM->mEntities);
    mESM->mEntities.getArchetype<Projector>()[0]->isMainCamera = true;

    InputStateEntity::upload(mESM->mEntities);
    UiStateEntity::upload(mESM->mEntities);
    auto &uiState = *mESM->mEntities.template getArchetype<UiState>()[0];
    uiState.isMonkeyMesh = mMonkeyMode;
    uiState.title = mTitle;

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
        auto &inputState = *mESM->mEntities.template getArchetype<InputState>()[0];
        if (inputState.closeWindow == IM_DOWN_EVENT)
            mWindowManager->close();
        if (inputState.toggleFullscreen == IM_DOWN_EVENT)
            mWindowManager->toggleFullscreen();

        // UI
        auto &uiState = *mESM->mEntities.template getArchetype<UiState>()[0];
        uiState.fps.update(mDeltaTime);
        uiState.cpuWaitTime = mCurrentCpuWaitTime;

        if (uiState.switchMesh) {
            mExitAfterMainLoop = false;
            mMonkeyMode = !mMonkeyMode;
            mWindowManager->close();
        }

        // Update camera Z for UI
        auto cameras = mESM->mEntities.template getArchetype<Projector, Transformer4>();
        for (auto & camera : cameras) {
            if (std::get<0>(camera)->isMainCamera) {
                uiState.cameraZ = std::get<1>(camera)->getPosition().z;
                break;
            }
        }

        mESM->mSystems.update(mDeltaTime);

        // Render
        if (uiState.returnToOriginalMeshBuffer)
            mRenderer->resetMesh();
        mCurrentCpuWaitTime = mRenderer->draw(mDeltaTime, mESM->mEntities);

        // Benchmark
        auto time = Timer::now();
        mDeltaTime = Timer::duration(mLastTimestamp, time);
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
    Log::i("Destroying Application");

    // Reset in order
    mRenderer.reset();
    mInputManager.reset();
    mWindowManager.reset();
    mESM.reset();
}
