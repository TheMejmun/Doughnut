
//#include "application.h"
//#include "io/printer.h"
#include "ecs/entity_manager.h"

#include <iostream>
#include <sstream>

struct test {
};

int main() {
    ECS::EntityManager<test, int, double, uint32_t> em;
    std::cout << "Created EM" << std::endl;

    std::cout << "Entities: " << em.entityCount() << ", Components: " << em.componentCount<int>() << std::endl;

    auto id = em.makeEntity();
    std::cout << "Created Entity" << std::endl;

    std::cout << "Entities: " << em.entityCount() << ", Components: " << em.componentCount<int>() << std::endl;

    em.insertComponent(0, id);
    std::cout << "Inserted Component" << std::endl;

    std::cout << "Entities: " << em.entityCount() << ", Components: " << em.componentCount<int>() << std::endl;

    em.insertComponent(1, id);
    std::cout << "Inserted Component" << std::endl;

    std::cout << "Entities: " << em.entityCount() << ", Components: " << em.componentCount<int>() << std::endl;

    em.removeComponent<int>(id);
    std::cout << "Removed Component" << std::endl;

    std::cout << "Entities: " << em.entityCount() << ", Components: " << em.componentCount<int>() << std::endl;

    em.insertComponent(2, id);
    std::cout << "Inserted Component" << std::endl;

    std::cout << "Entities: " << em.entityCount() << ", Components: " << em.componentCount<int>() << std::endl;

    auto id2 = em.makeEntity();
    std::cout << "Created Entity" << std::endl;

    std::cout << "Entities: " << em.entityCount() << ", Components: " << em.componentCount<int>() << std::endl;

    em.insertComponent(3, id2);
    std::cout << "Inserted Component" << std::endl;

    std::cout << "Entities: " << em.entityCount() << ", Components: " << em.componentCount<int>() << std::endl;

    std::cout << em.getComponent<int>(id) << std::endl;
    std::cout << em.getComponent<int>(id2) << std::endl;

    em.removeEntity(id);
    std::cout << "Removed Entity" << std::endl;

    std::cout << "Entities: " << em.entityCount() << ", Components: " << em.componentCount<int>() << std::endl;

    std::cout << em.getComponent<int>(id2) << std::endl;

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