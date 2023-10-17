//
// Created by Saman on 26.08.23.
//

#include "util/timer.h"
#include "io/logger.h"

using namespace Doughnut::Timer;

void FPSCounter::update(double lastFrametime) {
    this->frametimesLastSecond.push_back(lastFrametime);

    while (totalTime() > 1) {
        this->frametimesLastSecond.pop_front();
    }
}

double FPSCounter::totalTime() {
    double total = 0.0;
    for (const auto &frametime: this->frametimesLastSecond) {
        total += frametime;
    }

    return total;
}

ScopeTracer::~ScopeTracer() {
    auto timeEnded = Doughnut::Timer::now();
    auto duration = Doughnut::Timer::duration(mTimeStarted, timeEnded);
    Doughnut::Log::t(mName, "took", duration, "s");
}
