//
// Created by Saman on 25.12.23.
//

#define STB_IMAGE_IMPLEMENTATION

#include "graphics/texture.h"
#include "io/logger.h"
#include "util/timer.h"
#include "util/importer.h"
#include "util/require.h"

#include <stb_image.h>

using namespace dn;

Texture::Texture(const std::string &filename)
        : mLayout{4, SubpixelStructure::RGBA, SubpixelFormat::SRGB} {
    // STBI_rgb_alpha for alpha in the future
    {
        trace_scope("Texture load")
        int width, height;
        std::string localFilename = doughnutLocal(filename);
        mDataPointer = stbi_load(localFilename.c_str(), &width, &height, nullptr, STBI_rgb_alpha);
        mWidth = width;
        mHeight = height;
    }

    if (!mDataPointer) {
        log::e("Failed to load texture", filename);
        log::flush();
        throw std::runtime_error("Failed to load texture!");
    } else {
        log::i("Loaded", filename, "with", mWidth, "*", mHeight);
    }
}

Texture::Texture(std::vector<uint8_t> &&data, uint32_t width, uint32_t height, TextureLayout layout)
        : mDataVector(data), mWidth(width), mHeight(height), mLayout(layout) {
    require(!mDataVector.empty(), "Texture was created with empty data");
    require(mWidth > 1 && mHeight > 1, "Texture was created with invalid size");
}

size_t Texture::size() const {
    return mWidth * mHeight * mLayout.bytesPerPixel;
}

const uint8_t *Texture::data() const {
    if (mDataVector.empty()) {
        require_d(mDataPointer != nullptr, "Texture data is null");
        return mDataPointer;
    } else {
        return mDataVector.data();
    }
}

Texture::~Texture() {
    log::d("Freed texture");
    if (mDataVector.empty()) {
        stbi_image_free(mDataPointer);
    }
}