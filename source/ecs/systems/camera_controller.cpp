//
// Created by Saman on 31.07.23.
//

#include "ecs/systems/camera_controller.h"
#include "io/input_manager.h"
#include "ecs/system_manager.h"
#include "physics/transformer.h"
#include "ecs/components/projector.h"

void CameraController::update(const double delta, EntityManagerSpec &entityManager) {
    InputState *inputState = entityManager.template requestAll<InputState>()[0];
    auto cameras = entityManager.template requestAll<Projector, Transformer4>();

    for (uint32_t i = 0; i < std::get<0>(cameras).size(); ++i) {
        if (std::get<0>(cameras)[i]->isMainCamera) {
            auto transform = std::get<1>(cameras)[i];

            int move = 0;
            if (inputState->moveForward == IM_DOWN_EVENT ||
                inputState->moveForward == IM_HELD) {
                move += 1;
            }
            if (inputState->moveBackward == IM_DOWN_EVENT ||
                inputState->moveBackward == IM_HELD) {
                move -= 1;
            }
            transform->translate(glm::vec3(0, 0, delta * move));
        }
    }
}
