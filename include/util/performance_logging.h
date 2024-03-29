//
// Created by Saman on 02.09.23.
//

#ifndef DOUGHNUT_PERFORMANCE_LOGGING_H
#define DOUGHNUT_PERFORMANCE_LOGGING_H

#include "util/timer.h"
#include "ecs/components/ui_state.h"

struct FrameTimes {
    double cpuWaitTime;
    double totalFrameTime;
};

struct MeshStatistics {
    size_t vertexCount;
    size_t triangleCount;
};

namespace PerformanceLogging {
    extern const double LOG_DURATION;

    void newFrame(const FrameTimes &frameTimes);

    void meshCalculationStarted();

    void meshCalculationFinished();

    void meshUploadStarted();

    void meshUploadFinished(const MeshStatistics &meshStatistics);

    void update(UiState *uiState);
}

#endif //DOUGHNUT_PERFORMANCE_LOGGING_H
