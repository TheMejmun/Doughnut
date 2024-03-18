
#include "io/logger.h"
#include "application.h"
#include "core/scheduler.h"
#include "io/resource_pool.h"
#include "graphics/renderer_v2.h"

#include <iostream>

int main() {
    dn::log::init(true, false, false, true);

#ifdef NDEBUG
    // In case assert was used instead of dn::require
    dn::log::i("Do not run tests in Release mode.");
    return 1;
#endif

    trace_scope("Unit Tests")

    dn::testEntityManager();
    dn::testSystemManager();
    dn::testScheduler();

    // throw std::runtime_error("Hello World");
    return 0;
}