//
// Created by Sam on 2024-01-23.
//

#ifndef DOUGHNUT_QUEUE_FAMILY_INDICES_H
#define DOUGHNUT_QUEUE_FAMILY_INDICES_H

#include <optional>

namespace dn::vulkan {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> transferFamily;

        [[nodiscard]] bool isComplete() const;

        [[nodiscard]] bool isUnifiedGraphicsPresentQueue() const;

        [[nodiscard]] bool hasUniqueTransferQueue() const;

        void print();
    };
}

#endif //DOUGHNUT_QUEUE_FAMILY_INDICES_H
