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

namespace dn {
    /**
     * @tparam T    The template argument T must be constructible with only a string parameter,
     *              and have a size_t size() member function that returns an estimate for how much memory this resource uses.
     */

    template<class T>
    class ResourcePool {
    public:
        void preload(const std::string &fileHandle) {
            static_assert(std::is_constructible<T, const std::string &>::value
                          || std::is_constructible<T, const char *>::value,
                          "The chosen type is not constructible using a string!");

            std::lock_guard<std::mutex> guard(mPreloadMutex);
            if(isLoaded(fileHandle)) return;

            // Instantiate
            if constexpr (std::is_constructible<T, const std::string &>::value) {
                dn::log::v("Loading", typeid(T).name(), "with string", fileHandle);
                mResources[fileHandle] = new T{fileHandle};
            } else {
                dn::log::v("Loading", typeid(T).name(), "with char pointer", fileHandle);
                mResources[fileHandle] = new T{fileHandle.c_str()};
            }

            // Set size
            mSizes[fileHandle] = mResources[fileHandle]->size();
            dn::log::d("Caluclated size via size() as", mSizes[fileHandle], "B");
        }

        T *get(const std::string &fileHandle) {
            if (mResources[fileHandle] == nullptr) {
                preload(fileHandle);
            }

            return mResources[fileHandle];
        }

        // TODO request specific amount to be freed

        // TODO get amount of memory used by this pool

        size_t size() {
            size_t sum = 0;
            for (const auto &[key, value]: mSizes) {
                sum += value;
            }
            return sum;
        }

        void free(const std::string &fileHandle) {
            delete mResources[fileHandle];
            mSizes[fileHandle] = 0;
        }

        size_t sizeOf(const std::string &fileHandle) {
            return mSizes[fileHandle];
        }

        bool isLoaded(const std::string &fileHandle) {
            // Don't care about the fileHandle being inserted into the map, as it is likely to be used soon anyway if isLoaded is called.
            return mResources[fileHandle] != nullptr;
        }

        ~ResourcePool() {
            std::lock_guard<std::mutex> guard(mPreloadMutex);
            for (auto &[key, value]: mResources) {
                free(key);
            }
        }

    private:
        std::unordered_map<std::string, T *> mResources{};
        std::unordered_map<std::string, size_t> mSizes{};

        std::mutex mPreloadMutex{};

        // TODO last accessed map?
    };
}

#endif //DOUGHNUT_RESOURCE_POOL_H
