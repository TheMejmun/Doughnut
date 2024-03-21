//
// Created by Sam on 2023-04-09.
//

#ifndef DOUGHNUT_TRIANGLE_H
#define DOUGHNUT_TRIANGLE_H

#include "preprocessor.h"
#include "ecs/components/render_mesh.h"
#include "graphics/colors.h"

class Triangle {
public:
    RenderMesh mesh{
            .vertices{
                    {{0.f,   -0.5f,   0.5f},
                            Color::fromRGB({0.0f, 1.0f, 0.0f}).setLumaLab(90).getLAB()},
                    {{0.5f,  0.5f, 0.5f},
                            Color::fromRGB({0.0f, 0.0f, 1.0f}).setLumaLab(40).getLAB()},
                    {{-0.5f,  0.5f,  0.5f},
                            Color::fromRGB({1.0f, 0.0f, 0.0f}).setLumaLab(40).getLAB()}
            },
            .indices{
                    0, 1, 2,
                    0, 2, 1
            }};
};

#endif //DOUGHNUT_TRIANGLE_H
