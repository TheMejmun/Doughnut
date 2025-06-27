//
// Created by Saman on 01.04.24.
//

#ifndef ONEOVERNEGATIVE_FILESYSTEM_H
#define ONEOVERNEGATIVE_FILESYSTEM_H

#include <vector>

namespace dn {
    [[nodiscard]] const char *openFileDialog(const char *title, std::vector<const char *> filterPatterns);
}

#endif //ONEOVERNEGATIVE_FILESYSTEM_H
