//
// Created by Saman on 01.08.23.
//

#include "ecs/systems/sphere_controller.h"

void SphereController::update(double delta, EntityManagerSpec &entityManager) {
    const auto inputState = entityManager.getArchetype<InputState>()[0].components;

    if (inputState->toggleRotation == IM_DOWN_EVENT) {
        mDoSphereRotation = !mDoSphereRotation;
    }

    if (mDoSphereRotation) {
        auto spheres = entityManager.getArchetype<RotatingSphere, dn::Transform>();

        for (auto &sphere: spheres) {
            if (sphere.get<RotatingSphere>()->isRotatingSphere) { // Kinda redundant check
                sphere.get<dn::Transform>()->rotate(
                        glm::radians(15.0f * static_cast<float >(delta)),
                        glm::vec3(0, 1, 0));
            }
        }
    }
}
