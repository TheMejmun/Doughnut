//
// Created by Saman on 26.12.23.
//

#ifndef DOUGHNUT_RESOURCE_POOL_H
#define DOUGHNUT_RESOURCE_POOL_H

#include "logger.h"
#include "graphics/texture.h"

#include <unordered_map>
#include <string>

namespace Doughnut {
    template<class T>
    class ResourcePool {
    public:
        void preload(const std::string &fileHandle) {
            loadHelper<T>(fileHandle);
        }

        T *get(const std::string &fileHandle) {
            if (mResources[fileHandle] == nullptr) {
                loadHelper<T>(fileHandle);
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

        template<class S>
        inline void loadHelper(const std::string &fileHandle) {
            std::stringstream stream{};
            stream << "Tried to load unknown type" << typeid(T).name();
            std::string out = stream.str();
            Doughnut::Log::e(out);
            throw std::runtime_error(out);
        }

        template<>
        inline void loadHelper<Doughnut::Graphics::Texture>(const std::string &fileHandle) {
            Doughnut::Log::i("Template specialization for Texture hit!");
            mResources[fileHandle] = new Doughnut::Graphics::Texture{fileHandle};
            Doughnut::Log::i("Initialized Texture with", fileHandle);
        };
    };
}

#endif //DOUGHNUT_RESOURCE_POOL_H
