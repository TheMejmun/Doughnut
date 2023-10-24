//
// Created by Saman on 01.08.23.
//

#include "ecs/systems/sphere_controller.h"

void SphereController::update(double delta, EntityManagerSpec &entityManager) {
    auto &inputState = *entityManager.getArchetype<InputState>()[0];

    if (inputState.toggleRotation == IM_DOWN_EVENT) {
        mDoSphereRotation = !mDoSphereRotation;
    }

    if (mDoSphereRotation) {
        auto spheres = entityManager.getArchetype<RotatingSphere, Transformer4>();

        for (auto &sphere: spheres) {
            if (std::get<0>(sphere)->isRotatingSphere) { // Kinda redundant check
                std::get<1>(sphere)->rotate(
                        glm::radians(15.0f * static_cast<float >(delta)),
                        glm::vec3(0, 1, 0));
            }
        }
    }
}
