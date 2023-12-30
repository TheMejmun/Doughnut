//
// Created by Saman on 26.12.23.
//

#ifndef DOUGHNUT_RESOURCE_POOL_H
#define DOUGHNUT_RESOURCE_POOL_H

#include "logger.h"
#include "graphics/texture.h"

#include <unordered_map>
#include <string>
#include <type_traits>

namespace Doughnut {
    template<class T>
    class ResourcePool {
    public:
        void preload(const std::string &fileHandle) {
            static_assert(std::is_constructible<T, const std::string &>::value
                          || std::is_constructible<T, const char *>::value,
                          "The chosen type is not constructible using a string!");

            if constexpr (std::is_constructible<T, const std::string &>::value) {
                Doughnut::Log::v("Loading", typeid(T).name(), "with string", fileHandle);
                mResources[fileHandle] = new T{fileHandle};
            } else {
                Doughnut::Log::v("Loading", typeid(T).name(), "with char pointer", fileHandle);
                mResources[fileHandle] = new T{fileHandle.c_str()};
            }
        }

        T *get(const std::string &fileHandle) {
            if (mResources[fileHandle] == nullptr) {
                preload(fileHandle);
            }

            return mResources[fileHandle];
        }

        // TODO request specific amount to be freed

        // TODO get amount of memory used by this pool

        void free(const std::string &fileHandle) {
            delete mResources[fileHandle];
        }

        ~ResourcePool() {
            for (auto &[key, value]: mResources) {
                free(key);
            }
        }

    private:
        std::unordered_map<std::string, T *> mResources{};

        // TODO last accessed map?
    };
}

#endif //DOUGHNUT_RESOURCE_POOL_H
