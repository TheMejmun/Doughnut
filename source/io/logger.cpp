//
// Created by Sam on 2023-10-15.
//

#include "io/logger.h"

#include <mutex>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace Doughnut;

Scheduler Log::Internal::scheduler{1};

bool iInfoEnabled = false;
bool iDebugEnabled = false;
bool iVerboseEnabled = false;
bool iTraceEnabled = false;

void Log::init(bool enableInfo, bool enableDebug, bool enableVerbose, bool enableTrace) {
    iInfoEnabled = enableInfo;
    iDebugEnabled = enableDebug;
    iVerboseEnabled = enableVerbose;
    iTraceEnabled = enableTrace;
}

bool Log::infoEnabled() {
    return iInfoEnabled;
}

bool Log::debugEnabled() {
    return iDebugEnabled;
}

bool Log::verboseEnabled() {
    return iVerboseEnabled;
}

bool Log::traceEnabled() {
    return iTraceEnabled;
}

std::string getTimestamp() {
    time_t rawTime;
    time(&rawTime);

    struct tm timeInfo{};
    localtime_s(&timeInfo, &rawTime);

    std::stringstream stream{};
    stream << "["
           << (char) ('0' + (timeInfo.tm_hour / 10)) << (char) ('0' + (timeInfo.tm_hour % 10)) << ":"
           << (char) ('0' + (timeInfo.tm_min / 10)) << (char) ('0' + (timeInfo.tm_min % 10)) << ":"
           << (char) ('0' + (timeInfo.tm_sec / 10)) << (char) ('0' + (timeInfo.tm_sec % 10)) << "]";

    return stream.str();
}

void Log::Internal::log(Log::Internal::Level level, const std::string &message) {
    auto time = getTimestamp();
    switch (level) {
        case INFO:
            std::cout << time << " I: " << message << "\n";
            break;
        case DEBUG:
            std::cout << time << " D: " << message << "\n";
            break;
        case VERBOSE:
            std::cout << time << " V: " << message << "\n";
            break;
        case TRACE:
            std::cout << time << " T: " << message << "\n";
            break;
        case ERROR:
            std::cerr << time << " E: " << message << std::endl;
            break;
    }
}