//
// Created by Sam on 2024-02-21.
//

#ifndef DOUGHNUT_RENDERABLE_H
#define DOUGHNUT_RENDERABLE_H

#include "physics/transformer.h"
#include "graphics/texture.h"

#include <string>

namespace dn {
    struct Renderable {
        Texture texture;
        std::string vertexShader;
        std::string fragmentShader;
        std::string model;
        Transform transform;
    };
}

#endif //DOUGHNUT_RENDERABLE_H
