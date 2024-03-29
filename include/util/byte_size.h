//
// Created by Saman on 17.04.23.
//

#ifndef DOUGHNUT_BYTE_SIZE_H
#define DOUGHNUT_BYTE_SIZE_H

#include "preprocessor.h"

#include <cstdint>

#define TO_KB(bytes) (static_cast<float>(bytes) / 1024.0f)
#define TO_MB(bytes) (static_cast<float>(bytes) / (1024.0f * 1024.0f))
#define TO_GB(bytes) (static_cast<float>(bytes) / (1024.0f * 1024.0f * 1024.0f))

#define FROM_KB(kilobytes) (kilobytes * 1024)
#define FROM_MB(megabytes) (megabytes * (1024 * 1024))
#define FROM_GB(gigabytes) (gigabytes * (1024 * 1024 * 1024))

#endif //DOUGHNUT_BYTE_SIZE_H
