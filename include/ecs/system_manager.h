//
// Created by Sam on 2023-09-27.
//

#ifndef DOUGHNUT_SYSTEM_MANAGER_H
#define DOUGHNUT_SYSTEM_MANAGER_H

#include "io/printer.h"

#include <array>
#include <memory>
#include <algorithm>
#include <execution>

namespace ECS2 {
    template<class ENTITY_MANAGER>
    class System {
    public:
        virtual void update(double delta, ENTITY_MANAGER &entityManager) = 0;
    };

    template<class ENTITY_MANAGER, uint32_t LAYERS>
    class SystemManager {
    public:
        SystemManager(ENTITY_MANAGER *entityManager) : mEntityManager(entityManager) {
            static_assert(0 < LAYERS, "There must be at least one layer.");
            info("Creating SystemManager");
        }

        ~SystemManager() {
            info("Destroying SystemManager");
        }

        template<class SYSTEM, uint32_t LAYER>
        void insertSystem() {
            static_assert(std::is_base_of<System<ENTITY_MANAGER>, SYSTEM>::value, "System not derived from System<EntityManager>.");
            static_assert(std::is_default_constructible<SYSTEM>::value, "System is not constructible.");
            static_assert(LAYER < LAYERS, "Requested layer does not exist.");

            mSystemVectorLayers[LAYER].push_back(std::make_unique<SYSTEM>());
        }

        void update(double delta) {
            for (auto &layer: mSystemVectorLayers) {
                std::for_each(
                        std::execution::par,
                        layer.begin(),
                        layer.end(),
                        [&](std::unique_ptr<System<ENTITY_MANAGER>> &system) {
                            system->update(delta, *mEntityManager);
                        }
                );
            }
        };

    private:
        ENTITY_MANAGER *mEntityManager;

        std::array<std::vector<std::unique_ptr<System<ENTITY_MANAGER>>>, LAYERS> mSystemVectorLayers{};
    };

    void testSystemManager() {
        struct TestComponent {
            volatile int constructed = 0;
            volatile int executions = 0;
        };
        EntityManager<TestComponent> em{};

        em.makeEntity();
        em.insertComponent(TestComponent{}, 0);
        auto &testEntity = *std::get<0>(em.requestAll<TestComponent>())[0];

        SystemManager<decltype(em), 3> sm{&em};

        volatile uint32_t instanceCount = 0;

        class TestSystem : public System<decltype(em)> {
        public:
            void update(double delta, ECS2::EntityManager<TestComponent> &entityManager) override {
                auto &testEntity = *std::get<0>(entityManager.requestAll<TestComponent>())[0];
                if (firstUpdate) {
                    testEntity.constructed += 1;
                    firstUpdate = false;
                }
                testEntity.executions += 1;
            }

        private:
            bool firstUpdate = true;
        };

        assert(testEntity.constructed == 0);
        assert(testEntity.executions == 0);

        sm.insertSystem<TestSystem, 0>();
        sm.insertSystem<TestSystem, 0>();
        sm.insertSystem<TestSystem, 1>();
        sm.insertSystem<TestSystem, 1>();
        sm.insertSystem<TestSystem, 2>();
        sm.insertSystem<TestSystem, 2>();

        sm.update(0);

        assert(testEntity.constructed == 6);
        assert(testEntity.executions == 6);

        sm.update(0);

        assert(testEntity.constructed == 6);
        assert(testEntity.executions == 12);

        std::cout << "SystemManager test successful." << std::endl;
    }
}
#endif //DOUGHNUT_SYSTEM_MANAGER_H
