//
// Created by Sam on 2023-10-15.
//

#ifndef DOUGHNUT_LOGGER_H
#define DOUGHNUT_LOGGER_H

#include "core/scheduler.h"

#include <string>
#include <sstream>

// TODO move printing into separate thread

namespace Doughnut::Log {
    namespace Internal {
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
                    [&] { stream << args << " "; }(),
                    ...
            );
            return stream.str();
        }

        void log(Level level, const std::string &message);

        template<typename... ARGS>
        void logFormattedAsync(Level level, ARGS &&... args) {
            Internal::scheduler.queue({[=]() {
                auto formatted = Internal::format(args...);
                Internal::log(level, formatted);
            }});
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
            Internal::logFormattedAsync(Internal::INFO, args...);
        }
    }

    template<typename... ARGS>
    inline void d(ARGS &&... args) {
        if (debugEnabled()) {
            Internal::logFormattedAsync(Internal::DEBUG, args...);
        }
    }

    template<typename... ARGS>
    inline void v(ARGS &&... args) {
        if (verboseEnabled()) {
            Internal::logFormattedAsync(Internal::VERBOSE, args...);
        }
    }

    template<typename... ARGS>
    inline void t(ARGS &&... args) {
        if (traceEnabled()) {
            Internal::logFormattedAsync(Internal::TRACE, args...);
        }
    }

    template<typename... ARGS>
    inline void e(ARGS &&... args) {
        Internal::logFormattedAsync(Internal::ERROR, args...);
    }
}

#endif //DOUGHNUT_LOGGER_H
