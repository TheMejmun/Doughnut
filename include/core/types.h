//
// Created by Saman on 31.10.23.
//

#ifndef DOUGHNUT_TYPES_H
#define DOUGHNUT_TYPES_H

#include <cstddef>

namespace Doughnut {
    template<class T>
    struct WithIndex {
        size_t index;
        T value;

        WithIndex(const size_t &index, T &value) : index(index), value(value) {}

        WithIndex(const size_t &index, T &&value) : index(index), value(std::move(value)) {}
    };
}

#endif //DOUGHNUT_TYPES_H
