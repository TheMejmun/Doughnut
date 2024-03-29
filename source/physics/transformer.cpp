//
// Created by Sam on 2023-04-15.
//

#include "physics/transformer.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

void dn::Transform::translate(glm::vec3 translation) {
    this->forward = glm::translate(this->forward, translation);
    this->inverse = glm::translate(this->inverse, -translation);
}

void dn::Transform::scale(float scale) {
    this->forward = glm::scale(this->forward, glm::vec3(scale));
    this->inverse = glm::scale(this->inverse, glm::vec3(1.0f / scale));
}

void dn::Transform::scale(glm::vec3 scale) {
    this->forward = glm::scale(this->forward, scale);
    this->inverse = glm::scale(this->inverse, 1.0f / scale);
}

void dn::Transform::rotate(float radians, glm::vec3 axis) {
    this->forward = glm::rotate(this->forward, radians, axis);
    this->inverse = glm::rotate(this->inverse, -radians, axis);
}

glm::vec3 dn::Transform::getPosition() const {
    auto vec4 = this->forward * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    return {vec4.x, vec4.y, vec4.z};
}