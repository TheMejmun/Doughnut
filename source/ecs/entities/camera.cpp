//
// Created by Sam on 2023-10-08.
//

#include "ecs/entities/camera.h"

void Camera::upload(EntityManagerSpec &em) {
    auto id = em.makeEntity();

    dn::Transform transform{};
    transform.translate(glm::vec3(0, 0, -2.65));
    em.insertComponent(transform, id);

    em.template insertComponent<Projector>(id);
}