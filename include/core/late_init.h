//
// Created by Sam on 2024-02-11.
//

#ifndef DOUGHNUTSANDBOX_LATE_INIT_H
#define DOUGHNUTSANDBOX_LATE_INIT_H

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
#endif //DOUGHNUTSANDBOX_LATE_INIT_H
