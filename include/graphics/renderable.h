//
// Created by Sam on 2024-02-21.
//

#ifndef DOUGHNUTSANDBOX_RENDERABLE_H
#define DOUGHNUTSANDBOX_RENDERABLE_H

#include "physics/transformer.h"

#include <string>

namespace dn {
    struct Renderable {
        std::string texture;
        std::string vertexShader;
        std::string fragmentShader;
        std::string model;
        Transform transform;
    };
}

#endif //DOUGHNUTSANDBOX_RENDERABLE_H
