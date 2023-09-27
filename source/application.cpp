//
// Created by Sam on 2023-04-08.
//

#include "application.h"
#include "io/printer.h"
#include "ecs/entities/dense_sphere.h"
#include "ecs/entities/monkey.h"
#include "ecs/entities/camera.h"
#include "ecs/systems/camera_controller.h"
#include "ecs/systems/sphere_controller.h"
#include "ecs/systems/mesh_simplifier_controller.h"
#include "util/performance_logging.h"

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

    mEcs = std::make_unique<ECS>();
    mWindowManager = std::make_unique<WindowManager>();
    mWindowManager->create(this->title);
    mInputManager = std::make_unique<InputController>();
    mInputManager->create(mWindowManager->window, *mEcs);
    mRenderer = std::make_unique<GFX::Renderer>(this->title, mWindowManager->window);

    mRenderer->getUiState()->isMonkeyMesh = mMonkeyMode;

    // Entities
    Camera camera{};
    camera.components.isMainCamera = true;
    camera.upload(*mEcs);

    if (mMonkeyMode) {
        Monkey monkey{};
        monkey.upload(*mEcs);
    } else {
        DenseSphere sphere{};
        sphere.upload(*mEcs);
    }
}

void Application::mainLoop() {
    while (!mWindowManager->shouldClose()) {

        // Input
        mInputManager->update(mDeltaTime, *mEcs);
        auto &inputState = *mEcs->requestEntities(InputController::EvaluatorInputManagerEntity)[0]->inputState;
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

        auto cameraPos = mEcs->requestEntities(CameraController::EvaluatorActiveCamera)[0]
                ->transform->getPosition();
        uiState->cameraZ = cameraPos.z;

        // Systems
        CameraController::update(mDeltaTime, *mEcs);
        SphereController::update(mDeltaTime, *mEcs);
        if (uiState->runMeshSimplifier)
            MeshSimplifierController::update(*mEcs, &uiState->meshSimplifierTimeTaken,
                                             &uiState->meshSimplifierFramesTaken);

        // Render
        if (uiState->returnToOriginalMeshBuffer)
            mRenderer->resetMesh();
        mCurrentCpuWaitTime = mRenderer->draw(mDeltaTime, *mEcs);

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

    CameraController::destroy();
    SphereController::destroy();
    MeshSimplifierController::destroy();

    // Reset in order
    mRenderer.reset();
    mInputManager.reset();
    mWindowManager.reset();
    mEcs.reset();
}
