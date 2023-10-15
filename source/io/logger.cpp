//
// Created by Sam on 2023-10-15.
//

#include "io/logger.h"

#include <mutex>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace Doughnut;

std::mutex iPrintMutex{};
bool iInfoEnabled = false;
bool iDebugEnabled = false;
bool iVerboseEnabled = false;

void Log::init(bool enableInfo, bool enableDebug, bool enableVerbose) {
    iInfoEnabled = enableInfo;
    iDebugEnabled = enableDebug;
    iVerboseEnabled = enableVerbose;
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

void Log::log(Log::Level level, const std::string &message) {
    auto time = getTimestamp();
    std::lock_guard<std::mutex> guard{iPrintMutex};
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
        case ERROR:
            std::cerr << time << " E: " << message << std::endl;
            break;
    }
}