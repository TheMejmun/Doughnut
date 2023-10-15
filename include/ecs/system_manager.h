//
// Created by Sam on 2023-09-27.
//

#ifndef DOUGHNUT_SYSTEM_MANAGER_H
#define DOUGHNUT_SYSTEM_MANAGER_H

#define NEW_SCHEDULER

#include "io/logger.h"
#include "entity_manager.h"
#include "util/os.h"

#ifdef NEW_SCHEDULER

#include "core/scheduler.h"

#else

#include <algorithm>

#endif

#include <array>
#include <memory>
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
            Doughnut::Log::i("Creating SystemManager");
        }

        ~SystemManager() {
            Doughnut::Log::i("Destroying SystemManager");
        }

        template<class SYSTEM, uint32_t LAYER>
        void insertSystem() {
            static_assert(std::is_base_of<System<ENTITY_MANAGER>, SYSTEM>::value,
                          "System not derived from System<EntityManager>.");
            static_assert(std::is_default_constructible<SYSTEM>::value, "System is not constructible.");
            static_assert(LAYER < LAYERS, "Requested layer does not exist.");

            mSystemVectorLayers[LAYER].push_back(std::make_unique<SYSTEM>());
        }

        void update(double delta) {
            for (auto &layer: mSystemVectorLayers) {
#ifdef NEW_SCHEDULER
                for (auto &system: layer) {
                    // TODO find a way to insert all jobs at once.
                    mScheduler.queue({
                                             [&]() {
                                                 system->update(delta, *mEntityManager);
                                             }
                                     });
                }
                mScheduler.await();
#else
                // TODO restore parallelism
                // std::execution::par not available on macOS
                std::for_each(
#ifndef OS_MAC
                        std::execution::par,
#endif
                        layer.begin(),
                        layer.end(),
                        [&](std::unique_ptr<System<ENTITY_MANAGER>> &system) {
                            system->update(delta, *mEntityManager);
                        }
                );
#endif
            }
        };

    private:
        ENTITY_MANAGER *mEntityManager;
        std::array<std::vector<std::unique_ptr<System<ENTITY_MANAGER>>>, LAYERS> mSystemVectorLayers{};
#ifdef NEW_SCHEDULER
        Doughnut::Scheduler mScheduler{};
#endif
    };

    void testSystemManager();
}
#endif //DOUGHNUT_SYSTEM_MANAGER_H
