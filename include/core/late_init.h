//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUT_LATE_INIT_H
#define DOUGHNUT_LATE_INIT_H

#include <optional>

namespace dn {
    template<class T>
    class LateInit : public std::optional<T> {
    public:
        // allow nullptr assignment to reset this optional
        // for use with std::exchange
        LateInit<T> &operator=(void *null) {
            this->reset();
            return *this;
        }

        void clear() {
            this->reset();
        }
    };
}
#endif //DOUGHNUT_LATE_INIT_H
