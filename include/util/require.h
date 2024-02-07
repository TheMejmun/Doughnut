//
// Created by Sam on 2024-01-23.
//

#ifndef DOUGHNUT_REQUIRE_H
#define DOUGHNUT_REQUIRE_H

#include <stdexcept>
#include <vulkan/vulkan.hpp>

namespace dn {
    inline void require(bool result, const char *error) {
        if (!result) throw std::runtime_error(error);
    }

    inline void require(vk::Result result, const char *error) {
        if (result != vk::Result::eSuccess) throw std::runtime_error(error);
    }

    inline void require(VkResult result, const char *error) {
        if (result != VK_SUCCESS) throw std::runtime_error(error);
    }
}

#endif //DOUGHNUT_REQUIRE_H
