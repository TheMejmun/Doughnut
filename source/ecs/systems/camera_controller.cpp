//
// Created by Saman on 31.07.23.
//

#include "ecs/systems/camera_controller.h"
#include "ecs/system_manager.h"
#include "physics/transformer.h"
#include "ecs/components/projector.h"

void CameraController::update(const double delta, EntityManagerSpec &entityManager) {
    const auto inputState = entityManager.template getArchetype<InputState>()[0].components;
    const auto cameras = entityManager.template getArchetype<Projector, dn::Transform>();

    for (const auto &camera: cameras) {
        if (camera.get<Projector>()->isMainCamera) {

            int move = 0;
            if (inputState->moveForward == IM_DOWN_EVENT ||
                inputState->moveForward == IM_HELD) {
                move += 1;
            }
            if (inputState->moveBackward == IM_DOWN_EVENT ||
                inputState->moveBackward == IM_HELD) {
                move -= 1;
            }

            camera.get<dn::Transform>()->translate(glm::vec3(0, 0, delta * move));
        }
    }
}
