//
// Created by Saman on 26.08.23.
//

#include "util/timer.h"
#include "io/logger.h"

using namespace dn;

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
    auto timeEnded = dn::now();
    auto duration = dn::duration(mTimeStarted, timeEnded);
    dn::log::t(mName, "took", lround(1000 * duration), "ms");
}
