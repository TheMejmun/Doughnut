//
// Created by Sam on 2023-04-15.
//

#include "ecs/components/projector.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

glm::mat4 Projector::getProjection(float aspectRatio) const {
    return glm::perspective(
            this->fovYRadians,
            aspectRatio,
            this->zNear,
            this->zFar);
}

// TODO static?
glm::mat4 Projector::getView(const dn::Transform &eye) const {
    auto position = eye.getPosition();

    auto centerVec4 = eye.forward * this->cameraFront;
    glm::vec3 center = {centerVec4.x, centerVec4.y, centerVec4.z};

    auto upVec4 = eye.forward * this->worldUp;
    glm::vec3 up = {upVec4.x, upVec4.y, upVec4.z};

    return glm::lookAt(position, center, up);
}