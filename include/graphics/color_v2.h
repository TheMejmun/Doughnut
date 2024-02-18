//
// Created by Sam on 2024-02-18.
//

#ifndef DOUGHNUTSANDBOX_COLOR_V2_H
#define DOUGHNUTSANDBOX_COLOR_V2_H

#include <array>
#include <string>
#include <sstream>
#include <iomanip>

namespace dn {
    enum ColorSpace {
        RGB,
        SRGB,
        CIEXYZ,
        CIELAB
    };

    template<ColorSpace COLORSPACE>
    class Color {
    public:
        Color(float v0, float v1, float v2)
                : mValues{v0, v1, v2} {};

        ~Color() = default;

        std::string toString() {
            std::stringstream sstream{};
            sstream << std::setprecision(2) << std::fixed;

            if constexpr (COLORSPACE == RGB) {
                sstream << "Color<RGB>";
            } else if constexpr (COLORSPACE == SRGB) {
                sstream << "Color<SRGB>";
            } else if constexpr (COLORSPACE == CIEXYZ) {
                sstream << "Color<CIEXYZ>";
            } else if constexpr (COLORSPACE == CIELAB) {
                sstream << "Color<CIELAB>";
            } else {
                throw std::runtime_error("Unknown Color Space");
            }

            sstream << "{" << mValues[0] << ", " << mValues[1] << ", " << mValues[2] << "}";

            return sstream.str();
        }

        std::array<float, 3> mValues;
    };


    Color<RGB> rgb();

    Color<SRGB> srgb();

    Color<CIEXYZ> xyz();

    Color<CIELAB> lab();

}

#endif //DOUGHNUTSANDBOX_COLOR_V2_H
