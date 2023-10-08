//
// Created by Sam on 2023-10-08.
//

#include "ecs/entity_manager.h"

using namespace ECS2;

void ECS2::testEntityManager() {
    struct test {
        uint32_t someValue = 0;
    };

    ECS2::EntityManager<test, int, double, uint32_t> em;
    assert(em.entityCount() == 0);
    assert(em.componentCount<int>() == 0);

    auto id = em.makeEntity();
    assert(em.entityCount() == 1);
    assert(em.componentCount<int>() == 0);

    em.insertComponent(0, id);
    assert(em.entityCount() == 1);
    assert(em.componentCount<int>() == 1);
    assert(em.getComponent<int>(id) == 0);

    em.insertComponent(1, id);
    assert(em.entityCount() == 1);
    assert(em.componentCount<int>() == 1);
    assert(em.getComponent<int>(id) == 1);

    em.removeComponent<int>(id);
    assert(em.entityCount() == 1);
    assert(em.componentCount<int>() == 0);

    em.insertComponent(2, id);
    assert(em.entityCount() == 1);
    assert(em.componentCount<int>() == 1);
    assert(em.getComponent<int>(id) == 2);

    auto id2 = em.makeEntity();
    assert(em.entityCount() == 2);
    assert(em.componentCount<int>() == 1);

    auto id3 = em.makeEntity();
    assert(em.entityCount() == 3);
    assert(em.componentCount<int>() == 1);

    em.insertComponent(3, id2);
    assert(em.entityCount() == 3);
    assert(em.componentCount<int>() == 2);
    assert(em.getComponent<int>(id2) == 3);
    assert(em.getComponent<int>(id) == 2);

    auto allInts = em.requestAll<int>();
    assert(allInts.size() == 2);
    // Test values
    assert((*allInts[0] == 2 && *allInts[1] == 3) || (*allInts[0] == 3 && *allInts[1] == 2));

    auto allDoubles = em.requestAll<double>();
    assert(allDoubles.empty());

    auto allIntsAndDoubles = em.requestAll<int, double>();
    assert(std::get<0>(allIntsAndDoubles).empty());
    assert(std::get<1>(allIntsAndDoubles).empty());

    em.removeEntity(id);
    assert(em.entityCount() == 2);
    assert(em.componentCount<int>() == 1);
    assert(em.getComponent<int>(id2) == 3);

    std::cout << "EntityManager test successful." << std::endl;
}

void ECS2::benchmark(uint32_t count) {
    struct Component1 {
        double one;
        double two;
        double three;
        double four;
    };
    struct Component2 {
        double one;
        double two;
        double three;
        double four;
    };
    struct Component3 {
        double one;
        double two;
        double three;
        double four;
    };
    struct Component4 {
        double one;
        double two;
        double three;
        double four;
    };
    struct Component5 {
        double one;
        double two;
        double three;
        double four;
    };
    struct Component6 {
        double one;
        double two;
        double three;
        double four;
    };
    struct Component7 {
        double one;
        double two;
        double three;
        double four;
    };
    struct Component8 {
        double one;
        double two;
        double three;
        double four;
    };

    EntityManager<Component1, Component2, Component3, Component4, Component5, Component6, Component7, Component8> em{};

    {
        auto _trace_before = Doughnut::Timer::now();
        for (uint32_t i = 0; i < count; ++i) {
            em.makeEntity();
        }
        auto _trace_after = Doughnut::Timer::now();
        auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
        std::cout << "MAKE ENTITIES" << ": " << _duration << std::endl;
    }

    {
        auto _trace_before = Doughnut::Timer::now();
        for (uint32_t i = 0; i < count; ++i) {
            em.insertComponent(Component1{}, i);
        }
        auto _trace_after = Doughnut::Timer::now();
        auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
        std::cout << "INSERT C1" << ": " << _duration << std::endl;
    }

    {
        auto _trace_before = Doughnut::Timer::now();
        for (uint32_t i = 0; i < count; ++i) {
            if (i % 2 == 0) {
                em.insertComponent(Component2{}, i);
            }
        }
        auto _trace_after = Doughnut::Timer::now();
        auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
        std::cout << "INSERT C2" << ": " << _duration << std::endl;
    }

    {
        auto _trace_before = Doughnut::Timer::now();
        for (uint32_t i = 0; i < count; ++i) {
            if (i % 3 == 0) {
                em.insertComponent(Component3{}, i);
            }
        }
        auto _trace_after = Doughnut::Timer::now();
        auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
        std::cout << "INSERT C3" << ": " << _duration << std::endl;
    }

    {
        auto _trace_before = Doughnut::Timer::now();
        auto requested = em.requestAll<Component1>();
        auto _trace_after = Doughnut::Timer::now();
        auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
        std::cout << "GET ARCHETYPE 0" << ": " << _duration << std::endl;
    }

    {
        auto _trace_before = Doughnut::Timer::now();
        auto requested = em.requestAll<Component1, Component2>();
        auto _trace_after = Doughnut::Timer::now();
        auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
        std::cout << "GET ARCHETYPE 1" << ": " << _duration << std::endl;
    }

    {
        auto _trace_before = Doughnut::Timer::now();
        auto requested = em.requestAll<Component1, Component2, Component3>();
        auto _trace_after = Doughnut::Timer::now();
        auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
        std::cout << "GET ARCHETYPE 2" << ": " << _duration << std::endl;
    }
}