//
// Created by Sam on 2024-03-19.
//

#ifndef DOUGHNUTSANDBOX_HANDLE_H
#define DOUGHNUTSANDBOX_HANDLE_H

#include "io/logger.h"

namespace dn::vulkan {
    template<class VK_TYPE>
    class Handle {
    public:
        Handle() {
            dn::log::d("Created", typeid(VK_TYPE).name());
        }

        Handle(const Handle &) = delete;

        Handle(Handle &&) = default;

        ~Handle() {
            dn::log::d("Destroyed", typeid(VK_TYPE).name());
        }

        VK_TYPE *operator->() {
            return &mVulkan;
        }

        const VK_TYPE *operator->() const {
            return &mVulkan;
        }

        VK_TYPE &operator*() {
            return mVulkan;
        }

        const VK_TYPE &operator*() const {
            return mVulkan;
        }

        VK_TYPE mVulkan;
    };
}

#endif //DOUGHNUTSANDBOX_HANDLE_H
