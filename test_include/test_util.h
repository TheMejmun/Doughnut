//
// Created by Sam on 2024-03-23.
//

#ifndef DOUGHNUTSANDBOX_TEST_UTIL_H
#define DOUGHNUTSANDBOX_TEST_UTIL_H

#include "io/logger.h"
#include <functional>
#include <mutex>
#include <iostream>

extern int testCounter;
extern int errorCounter;
extern std::mutex TQM;
extern std::vector<std::function<void()>> TQ;

#define TEST_REGISTRY \
    int testCounter = 0; \
    int errorCounter = 0; \
    std::mutex TQM{}; \
    std::vector<std::function<void()>> TQ{};

#define STATIC(name) \
    static void name(); \
    static int _##name##_var = (name(), 0) ; \
    static void name()

#define REGISTER(unit_name, body) STATIC(unit_name) { \
    const char* UNIT_NAME = #unit_name;               \
    std::lock_guard<std::mutex> GUARD{TQM}; \
    body \
}

#define TEST_BODY(unit_name, test_name, body) TQ.emplace_back([=](){ \
                    dn::log::i(UNIT_NAME, "\b:", #test_name); \
                    body                                     \
});

// https://stackoverflow.com/questions/13923425/c-preprocessor-concatenation-with-variable
#define TEST(test_name, body) TEST_BODY(unit_name, test_name, body)

static void expect(bool boolean, const char *label) {
    ++testCounter;

    if (boolean) {
        dn::log::i("\tO", label);
    } else {
        ++errorCounter;
        dn::log::i("\tX", label);
    }
}


#endif //DOUGHNUTSANDBOX_TEST_UTIL_H
