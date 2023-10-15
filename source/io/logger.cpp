//
// Created by Sam on 2023-10-15.
//

#include "io/logger.h"

#include <mutex>
#include <sstream>
#include <iostream>

std::mutex printMutex{};
bool iInfoEnabled = false;
bool iDebugEnabled = false;
bool iVerboseEnabled = false;

void Doughnut::initLogger(bool enableInfo, bool enableDebug, bool enableVerbose) {
    iInfoEnabled = enableInfo;
    iDebugEnabled = enableDebug;
    iVerboseEnabled = enableVerbose;
}

bool Doughnut::infoEnabled() {
    return iInfoEnabled;
}

bool Doughnut::debugEnabled() {
    return iDebugEnabled;
}

bool Doughnut::verboseEnabled() {
    return iVerboseEnabled;
}

std::string getTimestamp() {
    time_t rawTime;
    time(&rawTime);

    struct tm timeInfo{};
    localtime_s(&timeInfo, &rawTime);

    std::stringstream stream{};
    stream << "[" << timeInfo.tm_hour << ":" << timeInfo.tm_min << ":" << timeInfo.tm_sec << "]";

    return stream.str();
}

void Doughnut::log(Doughnut::LogLevel level, const std::string &message) {
    auto time = getTimestamp();
    switch (level) {
        case INFO:
            printMutex.lock();
            std::cout << time << " I: " << message << "\n";
            printMutex.unlock();
            break;
        case DEBUG:
            printMutex.lock();
            std::cout << time << " D: " << message << "\n";
            printMutex.unlock();
            break;
        case VERBOSE:
            printMutex.lock();
            std::cout << time << " V: " << message << "\n";
            printMutex.unlock();
            break;
        case ERROR:
            printMutex.lock();
            std::cerr << time << " E: " << message << std::endl;
            printMutex.unlock();
            break;
    }
}