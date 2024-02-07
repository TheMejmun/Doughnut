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

        size_t size();

    private:
        int mWidth = 0, mHeight = 0, mChannels = 0;
        bool mHasAlpha = false;
        stbi_uc *mData = nullptr;
    };
}

#endif //DOUGHNUT_TEXTURE_H
