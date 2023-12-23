//
// Created by Saman on 31.07.23.
//

#include "ecs/systems/camera_controller.h"
#include "io/input_manager.h"
#include "ecs/system_manager.h"
#include "physics/transformer.h"
#include "ecs/components/projector.h"

void CameraController::update(const double delta, EntityManagerSpec &entityManager) {
    const auto inputState = entityManager.template getArchetype<InputState>()[0].components;
    const auto cameras = entityManager.template getArchetype<Projector, Transformer4>();

    for (const auto &camera: cameras) {
        if (std::get<0>(camera.components)->isMainCamera) {
            const auto transform = std::get<1>(camera.components);

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
