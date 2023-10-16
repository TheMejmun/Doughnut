//
// Created by Sam on 2023-10-08.
//

#include "ecs/entities/ui_state_entity.h"

void UiStateEntity::upload(EntityManagerSpec &em) {
    auto id = em.makeEntity();

    UiState uiState{};
    em.insertComponent(uiState, id);
}