//
// Created by Saman on 01.08.23.
//

#include "ecs/systems/sphere_controller.h"

void SphereController::update(double delta, EntityManagerSpec &entityManager) {
    auto &inputState = *entityManager.requestAll<InputState>()[0];

    if (inputState.toggleRotation == IM_DOWN_EVENT) {
        mDoSphereRotation = !mDoSphereRotation;
    }

    if (mDoSphereRotation) {
        auto spheres = entityManager.requestAll<RotatingSphere, Transformer4>();

        for (uint32_t i = 0; i < std::get<0>(spheres).size(); ++i) {
            if (std::get<0>(spheres)[i]->isRotatingSphere) { // Kinda redundant check
                std::get<1>(spheres)[i]->rotate(
                        glm::radians(15.0f * static_cast<float >(delta)),
                        glm::vec3(0, 1, 0));
            }
        }
    }
}
