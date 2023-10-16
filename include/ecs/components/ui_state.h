//
// Created by Saman on 26.08.23.
//

#ifndef REALTIME_CELL_COLLAPSE_UI_STATE_H
#define REALTIME_CELL_COLLAPSE_UI_STATE_H

#include "preprocessor.h"
#include "util/timer.h"

#include "GLFW/glfw3.h"
#include <string>

struct UiState {
    std::string title{};

    float cameraZ = 0;

    Doughnut::Timer::FPSCounter fps{};
    double cpuWaitTime = 0.0;
    bool loggingStarted = false;
    Doughnut::Timer::Point loggingStartTime{};

    uint32_t currentMeshVertices = 0;
    uint32_t currentMeshTriangles = 0;
    bool isMonkeyMesh = false;
    bool switchMesh = false;

    double meshSimplifierTimeTaken = 0.0;
    uint32_t meshSimplifierFramesTaken = 0;
    bool runMeshSimplifier = false;
    bool returnToOriginalMeshBuffer = false;

    double meshUploadTimeTaken = 0.0;
};

#endif //REALTIME_CELL_COLLAPSE_UI_STATE_H
