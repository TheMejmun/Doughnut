//
// Created by Sam on 2023-04-15.
//

#ifndef DOUGHNUT_TRANSFORMER_H
#define DOUGHNUT_TRANSFORMER_H

#include "preprocessor.h"

#include <glm/glm.hpp>

class Transformer4 {
public:
    void translate(glm::vec3 translation);

    void scale(float scale);

    void scale(glm::vec3 scale);

    void rotate(float radians, glm::vec3 axis);

    glm::mat4 forward{1.0f}, inverse{1.0f};

    [[nodiscard]] glm::vec3 getPosition() const;
};

#endif //DOUGHNUT_TRANSFORMER_H
