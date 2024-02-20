//
// Created by Saman on 25.12.23.
//

#ifndef DOUGHNUT_TEXTURE_H
#define DOUGHNUT_TEXTURE_H

#include <string>
#include <utility>
#include <stb_image.h>
#include "io/logger.h"

namespace dn {
    class Texture {
    public:
        explicit Texture(const std::string &filename);

        ~Texture();

        [[nodiscard]] size_t size() const;

        stbi_uc *mData = nullptr;

        int mWidth = 0, mHeight = 0, mChannels = 4, mOriginalChannels = 0;
        bool mHasAlpha = false;
    };
}

#endif //DOUGHNUT_TEXTURE_H
