//
// Created by Sam on 2023-04-07.
//

#ifndef DOUGHNUT_IMPORTER_H
#define DOUGHNUT_IMPORTER_H

#include "preprocessor.h"
#include "graphics/vertex.h"

#include <vector>
#include <string>

namespace dn {
    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    std::string doughnutLocal(const std::string &filename);

    std::vector<char> readFile(const std::string &filename);

    Mesh importMesh(const std::string &filename);
}

#endif //DOUGHNUT_IMPORTER_H
