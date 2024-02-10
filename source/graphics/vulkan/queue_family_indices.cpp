//
// Created by Sam on 2024-01-23.
//

#include "graphics/vulkan/queue_family_indices.h"
#include "io/logger.h"

using namespace dn;
using namespace dn::vulkan;

bool QueueFamilyIndices::isComplete() const {
    return this->graphicsFamily.has_value() &&
           this->presentFamily.has_value() &&
           this->transferFamily.has_value();
}

bool QueueFamilyIndices::isUnifiedGraphicsPresentQueue() const {
    if (!this->graphicsFamily.has_value() ||
        !this->presentFamily.has_value())
        return false;
    return this->graphicsFamily.value() == this->presentFamily.value();
}

bool QueueFamilyIndices::hasUniqueTransferQueue() const {
    if (!this->graphicsFamily.has_value() ||
        !this->presentFamily.has_value())
        return false;
    return this->graphicsFamily.value() != this->transferFamily.value();
}

void QueueFamilyIndices::print() {
    log::d(
            "QueueFamilyIndices: Graphics:", this->graphicsFamily.value(), "Present:", this->presentFamily.value(), "Transfer:", this->transferFamily.value()
    );
}