//
// Created by Sam on 2024-03-19.
//

#ifndef DOUGHNUT_HANDLE_H
#define DOUGHNUT_HANDLE_H

#include "context.h"
#include "io/logger.h"

#include <typeinfo>
#include <utility>
#include <sstream>

// TODO try implementing stacktrace storage
#if defined __has_include
#  if __has_include (<stacktrace>)
#    include <stacktrace>
#  endif
#endif

namespace dn::vulkan {
    extern std::unordered_map<void *, std::string> debugInfos;

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

        std::string toString() {
            std::stringstream sstream;
            sstream << typeid(VK_TYPE).name() << " " << mVulkan;
            return sstream.str();
        }

        std::string toString(const std::string &additionalInfo) {
            std::stringstream sstream;
            sstream << typeid(VK_TYPE).name() << " " << mVulkan;
            sstream << " { " << additionalInfo << " }";
            return sstream.str();
        }

        void setDebugInfo(const std::string &additionalInfo) {
            this->registerDebug(additionalInfo);
        }

        VK_TYPE mVulkan;
        const CONFIG_TYPE mConfig;

        Context &mContext;

        bool mMoved = false;
    protected:
        void registerDebug() {
#ifdef ENABLE_VALIDATION_LAYERS
            debugInfos.emplace(mVulkan, this->toString());
            log::v("Registered debug info", debugInfos.at(mVulkan));
#endif
        }

        void registerDebug(const std::string &additionalInfo) {
#ifdef ENABLE_VALIDATION_LAYERS
            debugInfos.erase(mVulkan);
            debugInfos.emplace(mVulkan, this->toString(additionalInfo));
            log::v("Registered debug info with additional", debugInfos.at(mVulkan));
#endif
        }
    };
}

#endif //DOUGHNUT_HANDLE_H
