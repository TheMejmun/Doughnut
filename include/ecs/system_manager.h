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
                // TODO restore parallelism
                // std::execution::par not available on macOS
                std::for_each(
//                        std::execution::par,
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

    void testSystemManager() ;
}
#endif //DOUGHNUT_SYSTEM_MANAGER_H
