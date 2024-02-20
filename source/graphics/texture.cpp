//
// Created by Saman on 25.12.23.
//

#include "graphics/texture.h"
#include "io/logger.h"
#include "util/timer.h"

#include <stb_image.h>
#include <filesystem>

using namespace dn;

Texture::Texture(const std::string &filename) {
    // STBI_rgb_alpha for alpha in the future
    {
        trace_scope("Texture load")
        std::string localFilename = "external/Doughnut/";
        localFilename.append(filename);
        if (std::filesystem::exists(filename)) {
            mData = stbi_load(filename.c_str(), &mWidth, &mHeight, &mOriginalChannels, STBI_rgb_alpha);
        } else if (std::filesystem::exists(localFilename)) {
            mData = stbi_load(localFilename.c_str(), &mWidth, &mHeight, &mOriginalChannels, STBI_rgb_alpha);
        } else {
            auto message = filename;
            message.append(" could not be found");
            throw std::runtime_error(message);
        }
    }

    if (!mData) {
        dn::log::e("Failed to load texture", filename);
        throw std::runtime_error("Failed to load texture!");
    } else {
        dn::log::i("Loaded", filename, "with", mWidth, "*", mHeight, "and", mChannels, "channels");
    }
}

size_t Texture::size() const {
    // stbi_load loads 8 bit always, hence 1 byte per channel
    return mWidth * mHeight * mChannels;
}

Texture::~Texture() {
    dn::log::i("Freed texture");
    stbi_image_free(mData);
}