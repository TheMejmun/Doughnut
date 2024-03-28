//
// Created by Sam on 2024-03-23.
//

#ifndef DOUGHNUT_TEST_UTIL_H
#define DOUGHNUT_TEST_UTIL_H

#include "io/logger.h"
#include <functional>
#include <mutex>
#include <iostream>

extern int testCounter;
extern int testCaseCounter;
extern int errorCounter;
extern std::mutex TQM;
extern std::vector<std::function<void()>> TQ;

// https://stackoverflow.com/questions/13923425/c-preprocessor-concatenation-with-variable

#define TEST_REGISTRY \
    int testCounter = 0; \
    int testCaseCounter = 0; \
    int errorCounter = 0; \
    std::mutex TQM{}; \
    std::vector<std::function<void()>> TQ{};

#define STATIC(name) \
    static void name(); \
    static int _##name##_var = (name(), 0) ; \
    static void name()

#define REGISTER(unit_name) \
    static void _register_##unit_name##_body(const char *UNIT_NAME); \
    STATIC(_register_##unit_name) { \
        const char* UNIT_NAME = #unit_name; \
        std::lock_guard<std::mutex> GUARD{TQM}; \
        _register_##unit_name##_body(UNIT_NAME); \
    } \
    static void _register_##unit_name##_body(const char *UNIT_NAME)

#define TEST(test_name) \
    TQ.emplace_back([=](){ dn::log::i(UNIT_NAME, "\b:", #test_name); ++testCounter; }); \
    TQ.emplace_back([](){}); \
    TQ[TQ.size() - 1] = [=]()


static void expect(bool boolean, const char *message) {
    ++testCaseCounter;
    if (!boolean) {
        ++errorCounter;
        dn::log::i("\tError:", message);
    }
}


#endif //DOUGHNUT_TEST_UTIL_H
