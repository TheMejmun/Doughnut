//
// Created by Sam on 2023-04-15.
//

#ifndef DOUGHNUT_PROJECTOR_H
#define DOUGHNUT_PROJECTOR_H

#include "preprocessor.h"
#include "physics/transformer.h"

#include <glm/glm.hpp>

struct Projector {
    float fovYRadians = glm::radians(45.0f);
    float zNear = 0.1f;
    float zFar = 100.f;
    glm::vec4 worldUp = {0, -1, 0, 0};
    glm::vec4 cameraFront = {0, 0, 1, 1};

    bool isMainCamera = false;

    [[nodiscard]] glm::mat4 getProjection(float aspectRatio) const;

    [[nodiscard]] glm::mat4 getView(const dn::Transform &eye) const;
};

#endif //DOUGHNUT_PROJECTOR_H
