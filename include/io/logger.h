//
// Created by Sam on 2023-10-15.
//

#ifndef DOUGHNUT_LOGGER_H
#define DOUGHNUT_LOGGER_H

#include "core/scheduler.h"

#include <string>
#include <sstream>

// TODO move printing into separate thread

namespace dn::log {
    namespace internal {
        extern Scheduler scheduler;

        enum Level {
            INFO,
            DEBUG,
            VERBOSE,
            TRACE,
            ERROR
        };

        template<typename... ARGS>
        std::string format(ARGS &&... args) {
            std::stringstream stream{};
            (
                    (stream << args << " "),
                    ...
            );
            return stream.str();
        }

        void log(Level level, const std::string &message);

        template<typename... ARGS>
        void logFormattedAsync(Level level, ARGS &&... args) {
            internal::scheduler.queue([=]() {
                auto formatted = internal::format(args...);
                internal::log(level, formatted);
            });
        }
    }

    void init(bool enableInfo, bool enableDebug, bool enableVerbose, bool enableTrace);

    bool infoEnabled();

    bool debugEnabled();

    bool verboseEnabled();

    bool traceEnabled();

    template<typename... ARGS>
    inline void i(ARGS &&... args) {
        if (infoEnabled()) {
            internal::logFormattedAsync(internal::INFO, args...);
        }
    }

    template<typename... ARGS>
    inline void d(ARGS &&... args) {
        if (debugEnabled()) {
            internal::logFormattedAsync(internal::DEBUG, args...);
        }
    }

    template<typename... ARGS>
    inline void v(ARGS &&... args) {
        if (verboseEnabled()) {
            internal::logFormattedAsync(internal::VERBOSE, args...);
        }
    }

    template<typename... ARGS>
    inline void t(ARGS &&... args) {
        if (traceEnabled()) {
            internal::logFormattedAsync(internal::TRACE, args...);
        }
    }

    template<typename... ARGS>
    inline void e(ARGS &&... args) {
        internal::logFormattedAsync(internal::ERROR, args...);
    }

    void flush();

    void benchmarkLogger(size_t count);
}

#endif //DOUGHNUT_LOGGER_H
