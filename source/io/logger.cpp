//
// Created by Sam on 2023-10-15.
//

#include "io/logger.h"
#include "util/os.h"
#include "util/timer.h"

#include <mutex>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace dn;

Scheduler log::internal::scheduler{1};

bool iInfoEnabled = false;
bool iDebugEnabled = false;
bool iVerboseEnabled = false;
bool iTraceEnabled = false;

void log::init(bool enableInfo, bool enableDebug, bool enableVerbose, bool enableTrace) {
    iInfoEnabled = enableInfo;
    iDebugEnabled = enableDebug;
    iVerboseEnabled = enableVerbose;
    iTraceEnabled = enableTrace;
}

bool log::infoEnabled() {
    return iInfoEnabled;
}

bool log::debugEnabled() {
    return iDebugEnabled;
}

bool log::verboseEnabled() {
    return iVerboseEnabled;
}

bool log::traceEnabled() {
    return iTraceEnabled;
}

std::string getTimestamp() {
#ifdef OS_WINDOWS
    time_t rawTime;
    time(&rawTime);

    struct tm timeInfo{};
    localtime_s(&timeInfo, &rawTime);

#else
    time_t rawTime;
    time(&rawTime);

    struct tm timeInfo{};
    localtime_r(&rawTime, &timeInfo);

#endif
    std::stringstream stream{};
    stream << "["
           << (char) ('0' + (timeInfo.tm_hour / 10)) << (char) ('0' + (timeInfo.tm_hour % 10)) << ":"
           << (char) ('0' + (timeInfo.tm_min / 10)) << (char) ('0' + (timeInfo.tm_min % 10)) << ":"
           << (char) ('0' + (timeInfo.tm_sec / 10)) << (char) ('0' + (timeInfo.tm_sec % 10)) << "]";

    return stream.str();
}

void log::internal::log(log::internal::Level level, const std::string &message) {
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

void log::flush() {
    log::internal::scheduler.await();
}

void log::benchmarkLogger(size_t count) {
    {
        dn::ScopeTracer tracer1{"Execute Logs"};
        {
            dn::ScopeTracer tracer2{"Insert Logs"};

            for (uint32_t i = 0; i < count; ++i) {
                dn::log::i("TESTING", "LOGGER", "FOLDED", i);
            }
        }

        dn::log::internal::scheduler.await();
    }
}