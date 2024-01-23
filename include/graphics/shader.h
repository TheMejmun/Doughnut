//
// Created by Sam on 2024-01-24.
//

#ifndef DOUGHNUT_SHADER_H
#define DOUGHNUT_SHADER_H

#include <string>
#include <vector>

namespace Doughnut {
    class Shader {
    public:
        explicit Shader(const std::string &filename);

        ~Shader() = default;

        size_t size();

    private:
        std::vector<char> mRaw{};
    };
}

#endif //DOUGHNUT_SHADER_H
