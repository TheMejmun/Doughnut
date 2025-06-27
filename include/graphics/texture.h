//
// Created by Saman on 25.12.23.
//

#ifndef DOUGHNUT_TEXTURE_H
#define DOUGHNUT_TEXTURE_H

#include "io/logger.h"

#include <string>
#include <utility>
#include <vulkan/vulkan.hpp>
#include <optional>

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
        explicit Texture(const std::string &filename = "resources/textures/debug.png");

        Texture(const std::string &filename,
                std::vector<uint8_t> &&data,
                uint32_t width,
                uint32_t height,
                TextureLayout layout);

        Texture(Texture &&other);

        Texture &operator=(Texture &&other) noexcept;

        ~Texture();

        [[nodiscard]] size_t size() const;

        [[nodiscard]] const uint8_t *data() const;

        [[nodiscard]] double min();

        [[nodiscard]] double max();

        void printData(size_t count);

        std::string mFilename;
        uint32_t mWidth = 0, mHeight = 0;
        TextureLayout mLayout;

    private:
        void calculateMinMax();

        uint8_t *mDataPointer = nullptr;
        std::vector<uint8_t> mDataVector{};
        std::optional<std::array<double, 2>> mMinMaxValues{};
    };
}

#endif //DOUGHNUT_TEXTURE_H
