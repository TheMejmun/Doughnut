//
// Created by Saman on 25.12.23.
//

#ifndef DOUGHNUT_TEXTURE_H
#define DOUGHNUT_TEXTURE_H

#include "io/logger.h"
#include "util/require.h"

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

        [[nodiscard]] uint8_t bytesPerSubpixel() const;
    };

    class Texture {
    public:
        explicit Texture(const std::string &filename = "resources/textures/debug.png");

        Texture(std::string filename,
                const std::vector<uint8_t> &data,
                uint32_t width,
                uint32_t height,
                TextureLayout layout);

        Texture(std::string filename,
                void *data,
                uint32_t width,
                uint32_t height,
                TextureLayout layout);

        Texture(Texture &&other) noexcept;

        // Move assignment
        Texture &operator=(Texture &&other) noexcept;

        ~Texture();

        [[nodiscard]] size_t size() const;

        [[nodiscard]] double min();

        [[nodiscard]] double max();

        [[nodiscard]] Texture convertTo(const TextureLayout &layout) const;

        std::string mFilename;
        uint32_t mWidth = 0, mHeight = 0;
        TextureLayout mLayout{};

        // Supports float, double, uint8, uint16, and uint32
        void *mData = nullptr;

    private:
        void calculateMinMax();

        [[nodiscard]] Texture convertType(const TextureLayout &layout) const;

        std::optional<std::array<double, 2>> mMinMaxValues{};
    };

    // This is not really useful...
    template<TextureLayout LAYOUT>
    struct SubpixelType {
        using Type = typename std::conditional<LAYOUT.subpixelFormat == LINEAR_FLOAT,
                // FLOAT
                typename std::conditional<LAYOUT.bytesPerSubpixel() == 4,
                        float, // 4
                        typename std::conditional<LAYOUT.bytesPerSubpixel() == 8,
                                double, // 8
                                void // ERROR
                        >::type
                >::type,
                // UINT
                typename std::conditional<LAYOUT.bytesPerSubpixel() <= 2,
                        typename std::conditional<LAYOUT.bytesPerSubpixel() == 1,
                                uint8_t, // 1
                                uint16_t // 2
                        >::type,
                        typename std::conditional<LAYOUT.bytesPerSubpixel() == 4,
                                uint32_t, // 4
                                void // ERROR
                        >::type
                >::type
        >::type;
    };
}

#endif //DOUGHNUT_TEXTURE_H
