
#include "io/logger.h"
#include "application.h"
#include "core/scheduler.h"

#include <iostream>

int main() {
#ifndef NDEBUG
    Doughnut::Log::init(true, true, false, true);
#else
    Doughnut::Log::init(true, false, false, true);
#endif

//    Doughnut::ECS::benchmarkEntityManager(1'000'000);
//    return 0;

//    Doughnut::Log::benchmarkLogger(1'000'000);
//    return 0;

#ifndef NDEBUG
    {
        Doughnut::ECS::testEntityManager();
        Doughnut::ECS::testSystemManager();
        Doughnut::testScheduler();
    }
#endif

    Doughnut::Application app{"Hello World!"};

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}