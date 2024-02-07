//
// Created by Sam on 2023-04-08.
//

#ifndef DOUGHNUT_TIMER_H
#define DOUGHNUT_TIMER_H

#include "preprocessor.h"

#include <iostream>
#include <chrono>
#include <cmath>
#include <deque>

#define trace_scope(name) auto _tracer = dn::ScopeTracer(name);

namespace dn {
    using Clock = std::chrono::steady_clock;
    using Second = std::chrono::duration<double>;
    using Time = std::chrono::time_point<Clock, Second>;

    inline Time now() {
        return Clock::now();
    }

    inline double duration(const Time &time1, const Time &time2) {
        return std::abs((double) (time1 - time2).count());
    }

    inline uint32_t fps(double frameTimeSec) {
        return static_cast<uint32_t>(1.0 / frameTimeSec);
    }

    inline uint32_t fps(const Time &time1, const Time &time2) {
        return fps(duration(time1, time2));
    }

    class FPSCounter {
    public:
        void update(double lastFrametime);

        [[nodiscard]] inline uint32_t currentFPS() const {
            return this->frametimesLastSecond.size();
        }

        std::deque<double> frametimesLastSecond{};
    private:
        double totalTime();
    };

    class ScopeTracer {
    public:
        explicit ScopeTracer(const char *name) : mName(name) {}

        explicit ScopeTracer(const std::string &name) : mName(name.c_str()) {}

        ~ScopeTracer();

    private:
        const char *mName;
        const Time mTimeStarted = dn::now();
    };
};


#endif //DOUGHNUT_TIMER_H
