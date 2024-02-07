
#include "io/logger.h"
#include "application.h"
#include "core/scheduler.h"
#include "io/resource_pool.h"
#include "graphics/renderer_v2.h"

#include <iostream>

int main() {
#ifndef NDEBUG
    dn::log::init(true, true, true, true);
#else
    dn::log::init(true, false, false, true);
#endif

#ifndef NDEBUG
    {
        dn::testEntityManager();
        dn::testSystemManager();
        dn::testScheduler();
    }
#endif

    try {
        dn::Application app{"Hello World!"};
        app.run();

        return EXIT_SUCCESS;

    } catch (const std::exception &e) {
        dn::log::flush();
        throw e;
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}