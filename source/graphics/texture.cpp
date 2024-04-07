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
#include <utility>
#include <typeinfo>

using namespace dn;

uint8_t TextureLayout::bytesPerSubpixel() const {
    switch (subpixelStructure) {
        case MONO:
            return bytesPerPixel;
        case RGB:
            require_d(bytesPerPixel % 3 == 0, "TextureLayout is malformed");
            return bytesPerPixel / 3;
        case RGBA:
            require_d(bytesPerPixel % 4 == 0, "TextureLayout is malformed");
            return bytesPerPixel / 4;
        default:
        error("TextureLayout is malformed");
    }
}

template<class T>
bool isFloat() {
    return typeid(T) == typeid(float) || typeid(T) == typeid(double);
}

const std::type_info &calculateType(TextureLayout layout) {
    if (layout.subpixelFormat == LINEAR_FLOAT) {
        switch (layout.bytesPerSubpixel()) {
            case 4:
                return typeid(float);
            case 8:
                return typeid(double);
        }
    } else {
        switch (layout.bytesPerSubpixel()) {
            case 1:
                return typeid(uint8_t);
            case 2:
                return typeid(uint16_t);
            case 4:
                return typeid(uint32_t);
        }
    }

    error("Failed to calculate type for given TextureLayout");
}

template<class T>
std::array<double, 2> calculateTemplatedMinMax(T *data, size_t count) {
    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::min();

    for (size_t i = 0; i < count; ++i) {
        auto subpixel = static_cast<double>(data[i]);

        if (subpixel < minValue) {
            minValue = subpixel;
        }
        if (subpixel > maxValue) {
            maxValue = subpixel;
        }
    }

    log::v("Calculated image min", minValue, "/", "max", maxValue);
    return {minValue, maxValue};
}

template<class FROM, class TO>
TO *convertTypeTemplated(FROM *data, size_t count) {
    // float = (float) uint / (float) max uint
    // uint = (uint) (float * max uint)

    double maxTo = isFloat<TO>() ? 1.0 : std::numeric_limits<TO>::max();
    double maxFrom = isFloat<FROM>() ? 1.0 : std::numeric_limits<FROM>::max();

    double normFactor = maxTo / maxFrom;

    double preCastAddition = (isFloat<FROM>() && !isFloat<TO>()) ? 0.5 : 0.0;

    TO *out = (TO *) std::malloc(sizeof(TO) * count);

    for (size_t i = 0; i < count; ++i) {
        out[i] = static_cast<TO>(data[i] * normFactor + preCastAddition);
    }

    return out;
}

template<class FROM>
void *convertTypeTemplated(const std::type_info &to, FROM *data, size_t count) {
    if (to == typeid(float)) {
        return convertTypeTemplated<FROM, float>(data, count);
    } else if (to == typeid(double)) {
        return convertTypeTemplated<FROM, double>(data, count);
    } else if (to == typeid(uint8_t)) {
        return convertTypeTemplated<FROM, uint8_t>(data, count);
    } else if (to == typeid(uint16_t)) {
        return convertTypeTemplated<FROM, uint16_t>(data, count);
    } else if (to == typeid(uint32_t)) {
        return convertTypeTemplated<FROM, uint32_t>(data, count);
    }
}

void *convertTypeTemplated(const std::type_info &from, const std::type_info &to, void *data, size_t count) {
    if (from == typeid(float)) {
        return convertTypeTemplated(to, (float *) data, count);
    } else if (from == typeid(double)) {
        return convertTypeTemplated(to, (double *) data, count);
    } else if (from == typeid(uint8_t)) {
        return convertTypeTemplated(to, (uint8_t *) data, count);
    } else if (from == typeid(uint16_t)) {
        return convertTypeTemplated(to, (uint16_t *) data, count);
    } else if (from == typeid(uint32_t)) {
        return convertTypeTemplated(to, (uint32_t *) data, count);
    }
}

// TODO provide different loaders as options
Texture::Texture(const std::string &filename)
        : mFilename(filename), mLayout{4, SubpixelStructure::RGBA, SubpixelFormat::SRGB} {
    // STBI_rgb_alpha for alpha in the future
    {
        trace_scope("Texture load")
        int width, height;
        std::string localFilename = doughnutLocal(filename);
        mData = stbi_load(localFilename.c_str(), &width, &height, nullptr, STBI_rgb_alpha);
        mWidth = width;
        mHeight = height;
    }

    if (!mData) {
        log::e("Failed to load texture", filename);
        log::flush();
        throw std::runtime_error("Failed to load texture!");
    } else {
        log::i("Loaded", filename, "with", mWidth, "*", mHeight);
    }
}

Texture::Texture(std::string filename,
                 const std::vector<uint8_t> &data,
                 uint32_t width,
                 uint32_t height,
                 TextureLayout layout)
        : mFilename(std::move(filename)), mWidth(width), mHeight(height), mLayout(layout) {
    require(!data.empty(), "Texture was created with empty data");
    require(mWidth >= 1 && mHeight >= 1, "Texture was created with invalid size");

    mData = std::malloc(data.size());
    std::memcpy(mData, data.data(), data.size());
}

Texture::Texture(std::string filename,
                 void *data,
                 uint32_t width,
                 uint32_t height,
                 TextureLayout layout)
        : mFilename(std::move(filename)), mWidth(width), mHeight(height), mLayout(layout) {
    require(mWidth >= 1 && mHeight >= 1, "Texture was created with invalid size");

    size_t size = width * height * layout.bytesPerPixel;
    mData = std::malloc(size);
    std::memcpy(mData, data, size);
}

size_t Texture::size() const {
    return mWidth * mHeight * mLayout.bytesPerPixel;
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
    const auto &type = calculateType(mLayout);
    if (type == typeid(float)) {
        mMinMaxValues = calculateTemplatedMinMax((float *) mData, mWidth * mHeight);
    } else if (type == typeid(double)) {
        mMinMaxValues = calculateTemplatedMinMax((double *) mData, mWidth * mHeight);
    } else if (type == typeid(uint8_t)) {
        mMinMaxValues = calculateTemplatedMinMax((uint8_t *) mData, mWidth * mHeight);
    } else if (type == typeid(uint16_t)) {
        mMinMaxValues = calculateTemplatedMinMax((uint16_t *) mData, mWidth * mHeight);
    } else if (type == typeid(uint32_t)) {
        mMinMaxValues = calculateTemplatedMinMax((uint32_t *) mData, mWidth * mHeight);
    }
}

Texture Texture::convertType(const TextureLayout &layout) const {
    return Texture{
            mFilename,
            convertTypeTemplated(calculateType(mLayout), calculateType(layout), mData, mWidth * mHeight),
            mWidth,
            mHeight,
            layout
    };
}

Texture Texture::convertTo(const TextureLayout &layout) const {
    // TODO channel transform
    // TODO SRGB transform
    return convertType(layout);
}

Texture &Texture::operator=(dn::Texture &&other) noexcept {
    if (this != &other) {
        log::v("Freeing", mFilename, "before move assignment of", other.mFilename);
        std::free(mData);

        mFilename = other.mFilename;
        mWidth = other.mWidth;
        mHeight = other.mHeight;
        mLayout = other.mLayout;
        mData = other.mData;
        other.mData = nullptr;
        if (other.mMinMaxValues.has_value()) {
            mMinMaxValues = other.mMinMaxValues;
        } else {
            mMinMaxValues.reset();
        }
    }
    return *this;
}

Texture::Texture(dn::Texture &&other) noexcept
        : mLayout() {
    log::v("Moving", other.mFilename);
    std::free(mData);

    mFilename = other.mFilename;
    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mLayout = other.mLayout;
    mData = other.mData;
    other.mData = nullptr;
    if (other.mMinMaxValues.has_value()) {
        mMinMaxValues = other.mMinMaxValues;
    } else {
        mMinMaxValues.reset();
    }
}

Texture::~Texture() {
    log::d("Freeing texture");
    std::free(mData);
}