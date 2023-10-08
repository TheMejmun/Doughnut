//
// Created by Sam on 2023-04-08.
//

#include "application.h"
#include "io/printer.h"
#include "ecs/entities/dense_sphere.h"
#include "ecs/entities/monkey.h"
#include "ecs/entities/camera.h"
#include "ecs/systems/camera_controller.h"
#include "util/performance_logging.h"
#include "ecs/systems/mesh_simplifier_controller.h"
#include "ecs/systems/sphere_controller.h"

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
    info("Creating Application");

#ifdef NDEBUG
    std::cout << this->title << " is running in release mode." << std::endl;
#else
    std::cout << this->title << " is running in debug mode." << std::endl;
#endif

    mESM = std::make_unique<EntitySystemManagerSpec>();
    mWindowManager = std::make_unique<WindowManager>(this->title);
    mInputManager = std::make_unique<InputController>(mWindowManager->window);
    mRenderer = std::make_unique<GFX::Renderer>(this->title, mWindowManager->window);

    mRenderer->getUiState()->isMonkeyMesh = mMonkeyMode;

    // Entities
    Camera::upload(mESM->mEntities);
    mESM->mEntities.requestAll<Projector>()[0]->isMainCamera = true;

    InputStateEntity::upload(mESM->mEntities);

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

        // Input
        mInputManager->update(mDeltaTime, mESM->mEntities);
        auto &inputState = *mESM->mEntities.template requestAll<InputState>()[0];
        if (inputState.closeWindow == IM_DOWN_EVENT)
            mWindowManager->close();
        if (inputState.toggleFullscreen == IM_DOWN_EVENT)
            mWindowManager->toggleFullscreen();

        // UI
        auto uiState = mRenderer->getUiState();
        uiState->fps.update(mDeltaTime);
        uiState->cpuWaitTime = mCurrentCpuWaitTime;

        if (uiState->switchMesh) {
            mExitAfterMainLoop = false;
            mMonkeyMode = !mMonkeyMode;
            mWindowManager->close();
        }

        // Update camera Z for UI
        auto cameras = mESM->mEntities.template requestAll<Projector, Transformer4>();
        for (uint32_t i = 0; i < std::get<0>(cameras).size(); ++i) {
            if (std::get<0>(cameras)[i]->isMainCamera) {
                uiState->cameraZ = std::get<1>(cameras)[i]->getPosition().z;
            }
        }

        // Systems
        // if (uiState->runMeshSimplifier) TODO

        mESM->mSystems.update(mDeltaTime);

        // Render
        if (uiState->returnToOriginalMeshBuffer)
            mRenderer->resetMesh();
        mCurrentCpuWaitTime = mRenderer->draw(mDeltaTime, mESM->mEntities);

        // Benchmark
        auto time = Timer::now();
        mDeltaTime = Timer::duration(mLastTimestamp, time);
        mLastTimestamp = time;

        // Performance logging
        PerformanceLogging::update(*uiState);
        PerformanceLogging::newFrame({
                                             .cpuWaitTime = mCurrentCpuWaitTime,
                                             .totalFrameTime = mDeltaTime
                                     });
    }
}

void Application::destroy() {
    info("Destroying Application");

    // Reset in order
    mRenderer.reset();
    mInputManager.reset();
    mWindowManager.reset();
    mESM.reset();
}
