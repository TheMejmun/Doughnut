
#include "application.h"
#include "ecs/entity_system_manager.h"
#include "ecs/entity_manager.h"
#include "ecs/system_manager.h"

#include <iostream>

int main() {
#ifndef NDEBUG
    ECS2::testEntityManager();
    ECS2::testSystemManager();
#endif

    Doughnut::Application app{};
    app.title = "Hello World!";

    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}