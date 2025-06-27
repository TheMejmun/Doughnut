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
        : mFilename(filename), mLayout{4, SubpixelStructure::RGBA, SubpixelFormat::SRGB} {
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

Texture::Texture(const std::string &filename,
                 std::vector<uint8_t> &&data,
                 uint32_t width,
                 uint32_t height,
                 TextureLayout layout)
        : mFilename(filename), mDataVector(data), mWidth(width), mHeight(height), mLayout(layout) {
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

double Texture::min() {
    if (!mMinMaxValues.has_value()) {
        calculateMinMax();
    }
    return mMinMaxValues->at(0);
}

double Texture::max() {
    if (!mMinMaxValues.has_value()) {
        calculateMinMax();
    }
    return mMinMaxValues->at(1);
}

void Texture::calculateMinMax() {
    double minValue = std::numeric_limits<float>::max();
    double maxValue = std::numeric_limits<float>::min();

    uint8_t bytesPerSubpixel;
    switch (mLayout.subpixelStructure) {
        case MONO:
            bytesPerSubpixel = mLayout.bytesPerPixel;
            break;
        case RGB:
            bytesPerSubpixel = mLayout.bytesPerPixel / 3;
            break;
        case RGBA:
            bytesPerSubpixel = mLayout.bytesPerPixel / 4;
            break;
    }

    const uint8_t *imageData = data();

    for (size_t i = 0; i < mWidth * mHeight * bytesPerSubpixel; ++i) {
        uint64_t subpixel = 0;
        for (uint8_t b = 0; b < bytesPerSubpixel; ++b) {
            subpixel = (subpixel << 8) + imageData[i * bytesPerSubpixel + b];
        }

        double floatSubpixel;
        if (mLayout.subpixelFormat == LINEAR_FLOAT) {
            switch (bytesPerSubpixel) {
                case 4: {
                    uint32_t shortenedSubpixel = subpixel;
                    floatSubpixel = *((float *) &shortenedSubpixel);
                }
                    break;
                case 8:
                    floatSubpixel = *((double *) &subpixel);
                    break;
                default:
                error("Float conversion of unknown precision");
            }
        } else {
            floatSubpixel = static_cast<double>(subpixel);
        }

        if (floatSubpixel < minValue) {
            minValue = floatSubpixel;
        }
        if (floatSubpixel > maxValue) {
            maxValue = floatSubpixel;
        }
    }

    mMinMaxValues.emplace(std::array<double, 2>({minValue, maxValue}));
    log::v("Calculated image min", mMinMaxValues->at(0), "/", "max", mMinMaxValues->at(1));
}

void Texture::printData(size_t count) {
    std::stringstream stream{};
    auto dataPointer = data();
    for (size_t i = 0; i < count && i < size(); ++i) {
        stream << " " << dataPointer[i];
    }
    log::i(stream.str());
}

Texture &Texture::operator=(dn::Texture &&other) noexcept {
    if (this != &other) {
        log::v("Freed", mFilename, "before move assignment of", other.mFilename);
        if (mDataVector.empty()) {
            stbi_image_free(mDataPointer);
        }

        mFilename = other.mFilename;
        mWidth = other.mWidth;
        mHeight = other.mHeight;
        mLayout = other.mLayout;
        mDataPointer = other.mDataPointer;
        other.mDataPointer = nullptr;
        mDataVector = std::move(other.mDataVector);
        other.mDataVector.clear();
        if (other.mMinMaxValues.has_value()) {
            mMinMaxValues = other.mMinMaxValues;
        } else {
            mMinMaxValues.reset();
        }
    }
    return *this;
}

Texture::Texture(dn::Texture &&other) {
    log::v("Moving", other.mFilename);
    if (mDataVector.empty()) {
        stbi_image_free(mDataPointer);
    }

    mFilename = other.mFilename;
    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mLayout = other.mLayout;
    mDataPointer = other.mDataPointer;
    other.mDataPointer = nullptr;
    mDataVector = std::move(other.mDataVector);
    other.mDataVector.clear();
    if (other.mMinMaxValues.has_value()) {
        mMinMaxValues = other.mMinMaxValues;
    } else {
        mMinMaxValues.reset();
    }
}

Texture::~Texture() {
    log::d("Freed texture");
    if (mDataVector.empty() && mDataPointer != nullptr) {
        stbi_image_free(mDataPointer);
    }
}