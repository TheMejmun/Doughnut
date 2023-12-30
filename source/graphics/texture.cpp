//
// Created by Saman on 25.12.23.
//

#include "graphics/texture.h"
#include "io/logger.h"
#include "util/timer.h"

#include <stb_image.h>

using namespace Doughnut::Graphics;

Texture::Texture(const std::string &filename) {
    // STBI_rgb_alpha for alpha in the future
    {
        trace_scope("Texture load")
        mData = stbi_load(filename.c_str(), &mWidth, &mHeight, &mChannels, STBI_rgb);
    }

    if (!mData) {
        Doughnut::Log::e("Failed to load texture", filename);
        Doughnut::Log::flush();
        throw std::runtime_error("Failed to load texture!");
    } else {
        Doughnut::Log::i("Loaded", filename);
    }
}

size_t Texture::size() {
    // stbi_load loads 8 bit always, hence 1 byte per channel
    return mWidth * mHeight * mChannels;
}

Texture::~Texture() {
    Doughnut::Log::i("Freed texture");
    stbi_image_free(mData);
}