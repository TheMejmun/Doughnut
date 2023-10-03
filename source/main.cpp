
//#include "application.h"
//#include "io/printer.h"
#include "ecs/entity_manager.h"

#include <iostream>
#include <sstream>

struct test {
};


int main() {
    ECS::testEntityManager();

    return 0;
//
//    Doughnut::Application app{};
//    app.title = "Hello World!";
//
//    try {
//        app.run();
//    } catch (const std::exception &e) {
//        std::cerr << e.what() << std::endl;
//        return EXIT_FAILURE;
//    }
//
//    return EXIT_SUCCESS;
}