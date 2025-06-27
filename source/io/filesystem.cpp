//
// Created by Saman on 01.04.24.
//

#include "io/filesystem.h"
#include <tinyfiledialogs.h>

const char *dn::openFileDialog(const char *title, std::vector<const char *> filterPatterns) {
    return tinyfd_openFileDialog(
            title,
            nullptr,
            static_cast<int>(filterPatterns.size()),
            filterPatterns.data(),
            nullptr,
            false);
}