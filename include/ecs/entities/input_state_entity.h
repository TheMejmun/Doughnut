//
// Created by Saman on 30.08.23.
//

#ifndef REALTIME_CELL_COLLAPSE_INPUT_STATE_ENTITY_H
#define REALTIME_CELL_COLLAPSE_INPUT_STATE_ENTITY_H

#include "preprocessor.h"
#include "ecs/components/input_state.h"

namespace InputStateEntity {
    template<class ENTITY_MANAGER>
    void upload(ENTITY_MANAGER &em) {
        auto id = em.makeEntity();

        InputState inputState{};
        em.insertComponent(inputState, id);
    }
}

#endif //REALTIME_CELL_COLLAPSE_INPUT_STATE_ENTITY_H
