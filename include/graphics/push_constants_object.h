//
// Created by Sam on 2024-02-26.
//

#ifndef DOUGHNUTSANDBOX_PUSH_CONSTANT_OBJECT_H
#define DOUGHNUTSANDBOX_PUSH_CONSTANT_OBJECT_H

#include "preprocessor.h"

#include <glm/glm.hpp>

// TODO does it really make sense for delta to be constant?
namespace dn {
    struct PushConstantsObject {
        alignas(16) glm::vec2 resolution;
    };
}

#endif //DOUGHNUTSANDBOX_PUSH_CONSTANT_OBJECT_H
