//
// Created by Sam on 2023-10-07.
//

#ifndef DOUGHNUT_ENTITY_SYSTEM_MANAGER_H
#define DOUGHNUT_ENTITY_SYSTEM_MANAGER_H

#include "entity_manager.h"
#include "system_manager.h"

namespace ECS2 {
    template<uint32_t SYSTEM_LAYERS, class... COMPONENTS>
    class EntitySystemManager {
    public:
        EntitySystemManager() : mSystems(&mEntities) {}

    private:
        EntityManager<COMPONENTS...> mEntities{};
        SystemManager<EntityManager<COMPONENTS...>, SYSTEM_LAYERS> mSystems;
    };
}

#endif //DOUGHNUT_ENTITY_SYSTEM_MANAGER_H
