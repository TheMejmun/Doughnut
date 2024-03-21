//
// Created by Sam on 2023-10-07.
//

#ifndef DOUGHNUT_ENTITY_SYSTEM_MANAGER_H
#define DOUGHNUT_ENTITY_SYSTEM_MANAGER_H

#include "entity_manager.h"
#include "system_manager.h"

namespace dn {
    template<uint32_t SYSTEM_LAYERS, class ENTITY_MANAGER>
    class EntitySystemManager {
    public:
        EntitySystemManager() : mSystems(&mEntities) {}

        ENTITY_MANAGER mEntities{};
        SystemManager<ENTITY_MANAGER, SYSTEM_LAYERS> mSystems;
    };
}

#endif //DOUGHNUT_ENTITY_SYSTEM_MANAGER_H
