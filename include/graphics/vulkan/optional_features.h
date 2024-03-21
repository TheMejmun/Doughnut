//
// Created by Sam on 2024-01-23.
//

#ifndef DOUGHNUT_OPTIONAL_FEATURES_H
#define DOUGHNUT_OPTIONAL_FEATURES_H

namespace dn::vulkan {
    struct OptionalFeatures {
        bool supportsWireframeMode = false;
        bool supportsAnisotropicFiltering = false;
    };
}

#endif //DOUGHNUT_OPTIONAL_FEATURES_H
