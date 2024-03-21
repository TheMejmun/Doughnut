//
// Created by Sam on 2023-04-09.
//

#ifndef DOUGHNUT_RENDER_MESH_H
#define DOUGHNUT_RENDER_MESH_H

#include "preprocessor.h"
#include "graphics/vertex.h"

#include <vector>

struct RenderMesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    bool isAllocated = false;
    int bufferIndex = 0;
};

#endif //DOUGHNUT_RENDER_MESH_H
