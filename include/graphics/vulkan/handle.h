//
// Created by Sam on 2024-03-19.
//

#ifndef DOUGHNUTSANDBOX_HANDLE_H
#define DOUGHNUTSANDBOX_HANDLE_H

#include "io/logger.h"

#include <typeinfo>

namespace dn::vulkan {
    template<class VK_TYPE, class CONFIG_TYPE>
    class Handle {
    public:
        Handle(Context &context, const CONFIG_TYPE &config)
                : mContext(context),
                  mConfig(config) {
            dn::log::d("Creating", typeid(VK_TYPE).name(), "handle");
        }

        Handle(const Handle &) = delete;

        Handle(Handle &&other) noexcept
                : mVulkan(std::exchange(other.mVulkan, nullptr)),
                  mConfig(other.mConfig),
                  mContext(other.mContext),
                  mMoved(false) {
            other.mMoved = true;
            dn::log::v("Moved", typeid(VK_TYPE).name(), "handle");
        }

        ~Handle() {
            dn::log::d("Destroyed", mMoved ? "moved" : "\b", typeid(VK_TYPE).name(), "handle");
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
        const CONFIG_TYPE mConfig;

        Context &mContext;

        bool mMoved = false;
    };
}

#endif //DOUGHNUTSANDBOX_HANDLE_H
