
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

    Doughnut::ECS::benchmark(1'000'000);
    return 0;

#ifndef NDEBUG
    {
        Doughnut::ECS::testEntityManager();
        Doughnut::ECS::testSystemManager();
        Doughnut::testScheduler();
    }
#endif



//    {
//        Doughnut::Timer::ScopeTracer tracer1{"Execute Logs"};
//        {
//            Doughnut::Timer::ScopeTracer tracer2{"Insert Logs"};
//
//            for (uint32_t i = 0; i < 100'000; ++i) {
//                Doughnut::Log::i("TESTING", "LOGGER", "FOLDED", i);
//            }
//        }
//
//        Doughnut::Log::Internal::scheduler.await();
//
//        return 0;
//    }

    Doughnut::Application app{"Hello World!"};

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}