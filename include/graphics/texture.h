//
// Created by Saman on 25.12.23.
//

#ifndef DOUGHNUT_TEXTURE_H
#define DOUGHNUT_TEXTURE_H

#include "io/logger.h"

#include <string>
#include <utility>
#include <vulkan/vulkan.hpp>

namespace dn {
    enum SubpixelStructure {
        MONO,
        RGB,
        RGBA
    };

    enum SubpixelFormat {
        LINEAR_FLOAT,
        LINEAR_UINT,
        SRGB
    };

    struct TextureLayout {
        uint8_t bytesPerPixel;
        SubpixelStructure subpixelStructure;
        SubpixelFormat subpixelFormat;
    };

    class Texture {
    public:
        explicit Texture(const std::string &filename);

        Texture(uint8_t *data, uint32_t width, uint32_t height, TextureLayout layout);

        ~Texture();

        [[nodiscard]] size_t size() const;

        uint8_t *mData = nullptr;

        uint32_t mWidth = 0, mHeight = 0;
        TextureLayout mLayout;
    };
}

#endif //DOUGHNUT_TEXTURE_H
