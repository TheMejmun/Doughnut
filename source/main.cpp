
#include "application.h"
#include <iostream>
#include "ecs/entity_manager.h"

int main() {
    ECS2::testEntityManager();
    ECS2::benchmark(1'000'000);
    return 0;

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