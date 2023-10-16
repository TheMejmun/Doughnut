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
        trace_scope("MAKE ENTITIES");
        for (uint32_t i = 0; i < count; ++i) {
            em.makeEntity();
        }
    }

    {
        trace_scope("INSERT C1");
        for (uint32_t i = 0; i < count; ++i) {
            em.insertComponent(Component1{}, i);
        }
    }

    {
        trace_scope("INSERT C2");
        for (uint32_t i = 0; i < count; ++i) {
            if (i % 2 == 0) {
                em.insertComponent(Component2{}, i);
            }
        }
    }

    {
        trace_scope("INSERT C3");
        for (uint32_t i = 0; i < count; ++i) {
            if (i % 3 == 0) {
                em.insertComponent(Component3{}, i);
            }
        }
    }

    {
        trace_scope("GET ARCHETYPE 0");
        auto requested = em.requestAll<Component1>();
    }

    {
        trace_scope("GET ARCHETYPE 1");
        auto requested = em.requestAll<Component1, Component2>();
    }

    {
        trace_scope("GET ARCHETYPE 2");
        auto requested = em.requestAll<Component1, Component2, Component3>();
    }
}