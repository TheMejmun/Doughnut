//
// Created by Sam on 2023-10-08.
//

#include "ecs/entity_manager.h"

using namespace dn;

void dn::benchmarkEntityManager(const size_t count) {
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
        for (size_t i = 0; i < count; ++i) {
            em.makeEntity();
        }
    }

    {
        trace_scope("INSERT C1");
        for (size_t i = 0; i < count; ++i) {
            em.insertComponent(Component1{}, i);
        }
    }

    {
        trace_scope("INSERT C2");
        for (size_t i = 0; i < count; ++i) {
            if (i % 2 == 0)
                em.insertComponent(Component2{}, i);
        }
    }

    {
        trace_scope("INSERT C3");
        for (size_t i = 0; i < count; ++i) {
            if (i % 3 == 0)
                em.insertComponent(Component3{}, i);
        }
    }

    {
        trace_scope("GET ARCHETYPE 0");
        auto requested = em.getArchetype<Component1>();
    }

    {
        trace_scope("GET ARCHETYPE 1");
        auto requested = em.getArchetype<Component1, Component2>();
    }

    {
        trace_scope("GET ARCHETYPE 2");
        auto requested = em.getArchetype<Component1, Component2, Component3>();
    }

    {
        trace_scope("GET & MODIFY C1 (BEFORE DELETE)");
        auto requested = em.getArchetype<Component1>();
        for (size_t i = 0; i < count / 2; ++i) {
            const auto component = requested[i].components;
            component->one += 1.0;
            component->two += 1.0;
            component->three += 1.0;
            component->four += 1.0;
        }
    }

    {
        trace_scope("REQUEST DELETE C1");
        for (size_t i = 0; i < count; ++i) {
            if (i % 2 == 0)
                em.requestComponentDeletion<Component1>(i);
        }
    }

    {
        trace_scope("COMMIT DELETE")
        em.commitDeletions();
    }

    {
        trace_scope("GET & MODIFY C1 (AFTER DELETE)");
        auto requested = em.getArchetype<Component1>();
        for (size_t i = 0; i < count / 2; ++i) {
            const auto component = requested[i].components;
            component->one += 1.0;
            component->two += 1.0;
            component->three += 1.0;
            component->four += 1.0;
        }
    }

    {
        trace_scope("REQUEST DELETE C2");
        for (size_t i = 0; i < count; ++i) {
            if (i % 2 == 0)
                em.requestComponentDeletion<Component2>(i);
        }
    }

    {
        trace_scope("REQUEST DELETE C3");
        for (size_t i = 0; i < count; ++i) {
            if (i % 2 == 0)
                em.requestComponentDeletion<Component3>(i);
        }
    }

    {
        trace_scope("COMMIT DELETE")
        em.commitDeletions();
    }
}