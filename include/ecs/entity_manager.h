//
// Created by Sam on 2023-09-27.
//

#ifndef DOUGHNUT_ENTITY_MANAGER_H
#define DOUGHNUT_ENTITY_MANAGER_H

#include "io/printer.h"

#include <cstdint>
#include <vector>
#include <map>
#include <typeindex>
#include <string>
#include <memory>
#include <iostream>
#include <array>
#include <optional>
#include <cassert>

namespace ECS {
    template<class... COMPONENTS>
    class EntityManager {
    public:
        EntityManager() {
            info("Creating EntityManager");
            init<COMPONENTS...>();
        }

        ~EntityManager() {
            info("Destroying EntityManager");
        }

        size_t makeEntity() {
            for (auto &v: mIndexVectors)
                assert(mDenseEntities.size() == v.size());

            size_t newIndex = std::numeric_limits<size_t>::max();

            for (size_t i = 0; i < mSparseEntities.size(); ++i) {
                if (mSparseEntities[i] == std::numeric_limits<size_t>::max()) {
                    newIndex = i;
                    i = std::numeric_limits<size_t>::max();
                }
            }

            if (newIndex == std::numeric_limits<size_t>::max()) {
                newIndex = mSparseEntities.size();
                mSparseEntities.push_back(0);
            }

            mSparseEntities[newIndex] = mDenseEntities.size();
            mDenseEntities.push_back(newIndex);
            for (auto &v: mIndexVectors)
                v.resize(v.size() + 1);

            return newIndex;
        }

        void removeEntity(size_t entity) {
            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            for (auto &v: mIndexVectors)
                assert(mDenseEntities.size() == v.size());
            assert(entity < mSparseEntities.size());

            size_t denseIndex = mSparseEntities[entity];
            assert(denseIndex < mDenseEntities.size());

            removeAllComponents<COMPONENTS...>(entity);

            size_t otherSparseIndex = mDenseEntities.back();
            mSparseEntities[otherSparseIndex] = denseIndex;
            mDenseEntities[denseIndex] = otherSparseIndex;
            mDenseEntities.pop_back();
            for (auto &v: mIndexVectors) {
                v[denseIndex] = v.back();
                v.pop_back();
            }

            mSparseEntities[entity] = std::numeric_limits<size_t>::max();
        }

        template<typename COMPONENT>
        void insertComponent(COMPONENT component, size_t entity) {
            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            size_t denseId = mSparseEntities[entity];
            size_t componentVectorId = mTypeIndexMap[std::type_index(typeid(COMPONENT))];

            if (!mIndexVectors[componentVectorId][denseId].has_value()) {
                verbose("Inserting component " << typeid(COMPONENT).name());
                mIndexVectors[componentVectorId][denseId] = componentVector<COMPONENT>().size();
                componentVector<COMPONENT>().emplace_back(component);
            } else {
                verbose("Overriding component " << typeid(COMPONENT).name());
                size_t componentId = mIndexVectors[componentVectorId][denseId].value();
                componentVector<COMPONENT>()[componentId] = std::move(component);
            }
        }

        template<typename COMPONENT>
        void removeComponent(size_t entity) {
            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            for (auto &v: mIndexVectors)
                assert(mDenseEntities.size() == v.size());
            assert(entity < mSparseEntities.size());

            const size_t denseIndex = mSparseEntities[entity];
            assert(denseIndex < mDenseEntities.size());

            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(COMPONENT))];
            std::optional<size_t> &componentIndex = mIndexVectors[typeIndex][denseIndex];
            if (componentIndex.has_value()) {
                auto currentLastIndex = componentVector<COMPONENT>().size() - 1;

                // Move current rear to deleted position and updated other entity with new component position
                componentVector<COMPONENT>()[componentIndex.value()] = componentVector<COMPONENT>().back();
                for (std::optional<size_t> &otherIndex: mIndexVectors[typeIndex]) {
                    if (otherIndex.has_value() && otherIndex.value() == currentLastIndex) {
                        otherIndex = componentIndex.value();
                        break;
                    }
                }

                componentVector<COMPONENT>().pop_back();
                componentIndex.reset();
            }
        }

        template<typename COMPONENT>
        COMPONENT getComponent(size_t entity) {
            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            const size_t denseIndex = mSparseEntities[entity];
            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(COMPONENT))];
            const std::optional<size_t> &componentIndex = mIndexVectors[typeIndex][denseIndex];

            assert(componentIndex.has_value());

            return componentVector<COMPONENT>()[componentIndex.value()];
        }

        template<typename COMPONENT>
        size_t componentCount() {
            return componentVector<COMPONENT>().size();
        }

        size_t entityCount() {
            return mDenseEntities.size();
        }

    private:
        std::vector<size_t> mSparseEntities{};
        std::vector<size_t> mDenseEntities{};
        std::array<std::vector<std::optional<size_t>>, sizeof...(COMPONENTS)> mIndexVectors{};

        std::map<std::type_index, size_t> mTypeIndexMap{};
        std::tuple<std::vector<COMPONENTS>...> mComponentVectors{};

        template<typename T>
        inline std::vector<T> &componentVector() {
            return std::get<std::vector<T>>(mComponentVectors);
        }

        template<class T, class T2, class... OTHER>
        void removeAllComponents(size_t entity) {
            removeComponent<T>(entity);

            removeAllComponents<T2, OTHER...>(entity);
        }

        template<class T>
        void removeAllComponents(size_t entity) {
            removeComponent<T>(entity);
        }

        template<class T, class T2, class... OTHER>
        void init() {
            initType<T>();

            init<T2, OTHER...>();
        }

        template<class T>
        void init() {
            initType<T>();
        }

        template<class T>
        void initType() {
            verbose("Adding type " << typeid(T).name());

            const auto typeIndex = std::type_index(typeid(T));
            assert(!mTypeIndexMap.contains(typeIndex) && "Can not add multiple components of the same type due to ambiguity");
            mTypeIndexMap[typeIndex] = mTypeIndexMap.size();
        }
    };
};

#endif //DOUGHNUT_ENTITY_MANAGER_H
