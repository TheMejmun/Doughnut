
#include "application.h"

#include <iostream>

int main() {
#ifndef NDEBUG
    ECS2::testEntityManager();
    ECS2::testSystemManager();
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