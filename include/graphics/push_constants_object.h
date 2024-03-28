//
// Created by Sam on 2024-02-26.
//

#ifndef DOUGHNUT_PUSH_CONSTANT_OBJECT_H
#define DOUGHNUT_PUSH_CONSTANT_OBJECT_H

#include "preprocessor.h"

#include <glm/glm.hpp>

// TODO does it really make sense for delta to be constant?
namespace dn {
    struct PushConstantsObject {
        alignas(16) glm::vec2 resolution;
    };
}

#endif //DOUGHNUT_PUSH_CONSTANT_OBJECT_H
