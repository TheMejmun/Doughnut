//
// Created by Sam on 2024-01-24.
//

#include "graphics/shader.h"
#include "util/importer.h"
#include "io/logger.h"
#include "util/require.h"
#include "util/timer.h"

#include <shaderc/shaderc.hpp>

using namespace dn;

Shader::Shader(const std::string &filename, ShaderType type)
        : mFilename(filename), mType(type) {
    auto in = readFile(filename);

    std::stringstream stream{};
    for (const char &c: in) {
        stream << c;
    }

    mReadable = stream.str();
}

void Shader::compile() {
    log::d("Compiling", mFilename);
    trace_function

    shaderc::Compiler compiler{};
    shaderc::CompileOptions options{};

    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    // TODO options.SetIncluder();

    shaderc_shader_kind kind;
    switch (mType) {
        case VERTEX:
            kind = shaderc_shader_kind::shaderc_vertex_shader;
            break;
        case FRAGMENT:
            kind = shaderc_shader_kind::shaderc_fragment_shader;
            break;
    }

    shaderc::SpvCompilationResult module =
            compiler.CompileGlslToSpv(mReadable, kind, mFilename.c_str(), options);

    require(module.GetCompilationStatus() == shaderc_compilation_status_success, module.GetErrorMessage().c_str());

    mBinary = {module.cbegin(), module.cend()};
}

size_t Shader::size() {
    return mBinary.size() * sizeof(uint32_t);
}