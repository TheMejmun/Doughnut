//
// Created by Sam on 2024-01-24.
//

#ifndef DOUGHNUT_SHADER_H
#define DOUGHNUT_SHADER_H

#include <string>
#include <vector>
#include <cstdint>

namespace dn {
    enum ShaderType {
        VERTEX,
        FRAGMENT
    };

    class Shader {
    public:
        Shader(const std::string &filename, ShaderType type);

        ~Shader() = default;

        size_t size();

        void compile();

        const std::string mFilename;
        std::string mReadable{};
        std::vector<uint32_t> mBinary{};
        const ShaderType mType;
    };
}

#endif //DOUGHNUT_SHADER_H
