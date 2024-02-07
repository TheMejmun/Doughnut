//
// Created by Sam on 2024-01-24.
//

#include "graphics/shader.h"
#include "util/importer.h"
#include "io/logger.h"

using namespace dn;

Shader::Shader(const std::string &filename) {
    mRaw = readFile(filename);

    if (log::verboseEnabled()) {
        std::stringstream stream{};
        for (const char &c: mRaw) {
            stream << c;
        }
       log::v(stream.str());
    }
}

size_t Shader::size() {
    return mRaw.size();
}