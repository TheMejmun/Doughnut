
//#include "application.h"
#include "ecs/entity_manager.h"
#include <iostream>

int main() {

    ECS::testEntityManager();
    ECS::benchmark();

//    Doughnut::Application app{};
//    app.title = "Hello World!";
//
//    try {
//        app.run();
//    } catch (const std::exception &e) {
//        std::cerr << e.what() << std::endl;
//        return EXIT_FAILURE;
//    }

    return EXIT_SUCCESS;
}