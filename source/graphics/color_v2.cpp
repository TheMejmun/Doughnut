//
// Created by Sam on 2024-02-18.
//

#include "graphics/color_v2.h"
#include "glm/ext/matrix_float3x3.hpp"

using namespace dn;

const glm::mat3 RGB_TO_XYZ_MAT = glm::mat3{
        0.4124564, 0.2126729, 0.0193339,
        0.3575761, 0.7151522, 0.1191920,
        0.1804375, 0.0721750, 0.9503041
};

const glm::mat3 XYZ_TO_RGB_MAT = glm::mat3{
        3.2404549, -0.9692665, 0.0556434,
        -1.5371389, 1.8760110, -0.2040258,
        -0.4985316, 0.0415561, 1.0572252,
};

const glm::vec3 D65_XYZ = {
        0.950489, 1.0, 1.088840
};

const glm::vec3 D50_XYZ = {
        0.964212, 1.0, 0.825188
};

constexpr double SRGB_GAMMA = 2.4;

// https://en.wikipedia.org/wiki/CIELAB_color_space
constexpr double LAB_DELTA = 6.0 / 29.0;
constexpr double LAB_DELTA_POW2 = LAB_DELTA * LAB_DELTA;
constexpr double LAB_DELTA_POW2_MUL3 = 3.0 * LAB_DELTA_POW2;
constexpr double LAB_DELTA_POW3 = LAB_DELTA_POW2 * LAB_DELTA;
constexpr double THIRD = 1.0 / 3.0;
constexpr double FOUR_29TH = 4.0 / 29.0;
constexpr double FIVEHUNDREDTH = 1.0 / 500.0;
constexpr double TWOHUNDREDTH = 1.0 / 200.0;
constexpr double ONEHUNDREDSIXTEENTH = 1.0 / 116.0;
