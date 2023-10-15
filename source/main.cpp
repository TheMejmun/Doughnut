
#include "io/logger.h"
#include "application.h"
#include "core/scheduler.h"

#include <iostream>

int main() {
#ifndef NDEBUG
    Doughnut::Log::init(true, true, false);
#endif
    Doughnut::Log::i("INFO");
    Doughnut::Log::d("DEBUG");
    Doughnut::Log::v("VERBOSE");
    Doughnut::Log::e("ERROR", "AAAAAAAAAAHHHH");

    return 0;

#ifndef NDEBUG
    {
        trace_scope("Unit Tests");
        ECS2::testEntityManager();
        ECS2::testSystemManager();
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