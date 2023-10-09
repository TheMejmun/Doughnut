//
// Created by Sam on 2023-10-08.
//

#include "ecs/entities/input_state_entity.h"

void InputStateEntity::upload(EntityManagerSpec &em) {
    auto id = em.makeEntity();

    InputState inputState{};
    em.insertComponent(inputState, id);
}