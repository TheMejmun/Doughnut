//
// Created by Sam on 2023-09-27.
//

#ifndef DOUGHNUT_SYSTEM_MANAGER_H
#define DOUGHNUT_SYSTEM_MANAGER_H

#include "io/printer.h"
#include "entity_manager.h"

#include <array>
#include <memory>
#include <algorithm>
#include <execution>

namespace ECS2 {
    template<class ENTITY_MANAGER>
    class System {
    public:
        virtual ~System() = default;

        virtual void update(double delta, ENTITY_MANAGER &entityManager) = 0;
    };

    template<class ENTITY_MANAGER, uint32_t LAYERS>
    class SystemManager {
    public:
        explicit SystemManager(ENTITY_MANAGER *entityManager) : mEntityManager(entityManager) {
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

    // TODO could fail due to race conditions in 'TestSystem::update'
    void testSystemManager() {
        EntityManager<int, long> em{};

        em.makeEntity();
        em.insertComponent<int>(0, 0);
        em.insertComponent<long>(0, 0);
        auto &executions = *std::get<0>(em.requestAll<int>())[0];
        auto &constructed = *std::get<0>(em.requestAll<long>())[0];

        SystemManager<decltype(em), 3> sm{&em};

        class TestSystem : public System<decltype(em)> {
        public:
            ~TestSystem() override = default;

            void update(double delta, decltype(em) &entityManager) override {
                auto &executions = *std::get<0>(entityManager.requestAll<int>())[0];
                auto &constructed = *std::get<0>(entityManager.requestAll<long>())[0];
                if (firstUpdate) {
                    constructed += 1;
                    firstUpdate = false;
                }
                executions += 1;
            }

        private:
            bool firstUpdate = true;
        };

        assert(constructed == 0);
        assert(executions == 0);

        sm.insertSystem<TestSystem, 0>();
        sm.insertSystem<TestSystem, 0>();
        sm.insertSystem<TestSystem, 1>();
        sm.insertSystem<TestSystem, 1>();
        sm.insertSystem<TestSystem, 2>();
        sm.insertSystem<TestSystem, 2>();

        sm.update(0);

        assert(constructed == 6);
        assert(executions == 6);

        sm.update(0);

        assert(constructed == 6);
        assert(executions == 12);

        std::cout << "SystemManager test successful." << std::endl;
    }
}
#endif //DOUGHNUT_SYSTEM_MANAGER_H
