//
// Created by Sam on 2023-10-15.
//

#ifndef DOUGHNUT_LOGGER_H
#define DOUGHNUT_LOGGER_H

#include <string>
#include <sstream>

// TODO move printing into separate thread

namespace Doughnut::Log {
    enum Level {
        INFO,
        DEBUG,
        VERBOSE,
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

    bool infoEnabled();

    bool debugEnabled();

    bool verboseEnabled();

    template<typename... ARGS>
    inline void i(ARGS &&... args) {
        if (infoEnabled()) {
            auto formatted = format(args...);
            log(INFO, formatted);
        }
    }

    template<typename... ARGS>
    inline void d(ARGS &&... args) {
        if (debugEnabled()) {
            auto formatted = format(args...);
            log(DEBUG, formatted);
        }
    }

    template<typename... ARGS>
    inline void v(ARGS &&... args) {
        if (verboseEnabled()) {
            auto formatted = format(args...);
            log(VERBOSE, formatted);
        }
    }

    template<typename... ARGS>
    inline void e(ARGS &&... args) {
        auto formatted = format(args...);
        log(ERROR, formatted);
    }

    void log(Level level, const std::string &message);

    void init(bool enableInfo, bool enableDebug, bool enableVerbose);
}

#endif //DOUGHNUT_LOGGER_H
