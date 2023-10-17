//
// Created by Sam on 2023-09-27.
//

#ifndef DOUGHNUT_ENTITY_MANAGER_H
#define DOUGHNUT_ENTITY_MANAGER_H

#include "io/logger.h"
#include "util/timer.h"

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

namespace ECS2 {
    template<class... COMPONENTS>
    class EntityManager {
    public:
        EntityManager() {
            Doughnut::Log::i("Creating EntityManager");
            init<COMPONENTS...>();
        }

        ~EntityManager() {
            Doughnut::Log::i("Destroying EntityManager");
        }

        uint32_t makeEntity() {
            assert(mDenseEntities.size() == mIndexArrays.size());

            uint32_t newIndex;

            if (mSparseEntities.size() == mDenseEntities.size()) {
                newIndex = mSparseEntities.size();
                mSparseEntities.emplace_back();
            } else {
                for (uint32_t i = 0; i < mSparseEntities.size(); ++i) {
                    if (mSparseEntities[i] == std::numeric_limits<uint32_t>::max()) {
                        newIndex = i;
                        break;
                    }
                }
            }

            mSparseEntities[newIndex] = mDenseEntities.size();
            mDenseEntities.emplace_back(newIndex);
            mIndexArrays.emplace_back();

            return newIndex;
        }

        void removeEntity(uint32_t entity) {
            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<uint32_t>::max()
                   && "Accessing non-existent entity.");

            assert(mDenseEntities.size() == mIndexArrays.size());
            assert(entity < mSparseEntities.size());

            uint32_t denseIndex = mSparseEntities[entity];
            assert(denseIndex < mDenseEntities.size());

            removeAllComponents<COMPONENTS...>(entity);

            uint32_t otherSparseIndex = mDenseEntities.back();
            mSparseEntities[otherSparseIndex] = denseIndex;
            mDenseEntities[denseIndex] = otherSparseIndex;
            mDenseEntities.pop_back();
            mIndexArrays[denseIndex] = mIndexArrays.back();
            mIndexArrays.pop_back();

            mSparseEntities[entity] = std::numeric_limits<uint32_t>::max();
        }

        template<class COMPONENT>
        void insertComponent(COMPONENT component, uint32_t entity) {
            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<uint32_t>::max()
                   && "Accessing non-existent entity.");

            uint32_t denseId = mSparseEntities[entity];
            uint32_t componentVectorId = mTypeIndexMap[std::type_index(typeid(COMPONENT))];

            if (!mIndexArrays[denseId][componentVectorId].has_value()) {
                Doughnut::Log::v("Inserting component ", typeid(COMPONENT).name());
                mIndexArrays[denseId][componentVectorId] = componentVector<COMPONENT>().size();
                componentVector<COMPONENT>().emplace_back(component);
            } else {
                Doughnut::Log::v("Overriding component ", typeid(COMPONENT).name());
                uint32_t componentId = *mIndexArrays[denseId][componentVectorId];
                componentVector<COMPONENT>()[componentId] = std::move(component);
            }
        }

        template<class COMPONENT>
        COMPONENT *insertComponent(uint32_t entity) {
            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<uint32_t>::max()
                   && "Accessing non-existent entity.");

            uint32_t denseId = mSparseEntities[entity];
            uint32_t componentVectorId = mTypeIndexMap[std::type_index(typeid(COMPONENT))];

            if (!mIndexArrays[denseId][componentVectorId].has_value()) {
                Doughnut::Log::v("Inserting component ", typeid(COMPONENT).name());
                mIndexArrays[denseId][componentVectorId] = componentVector<COMPONENT>().size();
                componentVector<COMPONENT>().emplace_back();
                return &componentVector<COMPONENT>().back();
            } else {
                Doughnut::Log::v("Overriding component ", typeid(COMPONENT).name());
                uint32_t componentId = *mIndexArrays[denseId][componentVectorId];
                componentVector<COMPONENT>().emplace(componentVector<COMPONENT>().begin() + componentId);
                return &componentVector<COMPONENT>()[componentId];
            }
        }

        template<class COMPONENT>
        void removeComponent(uint32_t entity) {
            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<uint32_t>::max()
                   && "Accessing non-existent entity.");

            assert(mDenseEntities.size() == mIndexArrays.size());
            assert(entity < mSparseEntities.size());

            const uint32_t denseIndex = mSparseEntities[entity];
            assert(denseIndex < mDenseEntities.size());

            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(COMPONENT))];
            std::optional<uint32_t> &componentIndex = mIndexArrays[denseIndex][typeIndex];
            if (componentIndex.has_value()) {
                auto currentLastIndex = componentVector<COMPONENT>().size() - 1;

                // Move current rear to deleted position and update other entity with new component position
                componentVector<COMPONENT>()[*componentIndex] = componentVector<COMPONENT>().back();
                for (auto &otherIndexArray: mIndexArrays) {
                    if (otherIndexArray[typeIndex].has_value() && *otherIndexArray[typeIndex] == currentLastIndex) {
                        otherIndexArray[typeIndex] = *componentIndex;
                        break;
                    }
                }

                componentVector<COMPONENT>().pop_back();
                componentIndex.reset();
            }
        }

        template<class COMPONENT>
        COMPONENT getComponent(uint32_t entity) {
            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<uint32_t>::max()
                   && "Accessing non-existent entity.");

            const uint32_t denseIndex = mSparseEntities[entity];
            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(COMPONENT))];
            const std::optional<uint32_t> &componentIndex = mIndexArrays[denseIndex][typeIndex];

            assert(componentIndex.has_value());

            return componentVector<COMPONENT>()[*componentIndex];
        }

        template<class T>
        std::vector<T *> requestAll() {
            std::vector<T *> out{};

            std::vector<bool> matchesArchetype{};
            matchesArchetype.resize(mDenseEntities.size(), true);

            collectArchetypeMatches<T>(matchesArchetype);

            uint32_t totalMatches = 0;
            for (auto b: matchesArchetype) {
                if (b) ++totalMatches;
            }

            out.resize(totalMatches);

            uint32_t archetypeIndex = 0;
            for (uint32_t denseIndex = 0; denseIndex < mDenseEntities.size(); ++denseIndex) {
                if (matchesArchetype[denseIndex]) {
                    insertSingleArchetypeComponents<T>(out, mIndexArrays[denseIndex], archetypeIndex);
                    ++archetypeIndex;
                }
            }

            return out;
        }

        template<class T, class T2, class... OTHER>
        std::tuple<std::vector<T *>, std::vector<T2 *>, std::vector<OTHER *>...> requestAll() {
            std::tuple<std::vector<T *>, std::vector<T2 *>, std::vector<OTHER *>...> out{};

            std::vector<bool> matchesArchetype{};
            matchesArchetype.resize(mDenseEntities.size(), true);

            collectArchetypeMatches<T, T2, OTHER...>(matchesArchetype);

            uint32_t totalMatches = 0;
            for (auto b: matchesArchetype) {
                if (b) ++totalMatches;
            }

            resizeArchetypes<std::tuple<std::vector<T *>, std::vector<T2 *>, std::vector<OTHER *>...>, T, T2, OTHER...>(out, totalMatches);

            uint32_t archetypeIndex = 0;
            for (uint32_t denseIndex = 0; denseIndex < mDenseEntities.size(); ++denseIndex) {
                if (matchesArchetype[denseIndex]) {
                    insertArchetypeComponents<std::tuple<std::vector<T *>, std::vector<T2 *>, std::vector<OTHER *>...>, T, T2, OTHER...>(out, mIndexArrays[denseIndex], archetypeIndex);
                    ++archetypeIndex;
                }
            }

            return out;
        }

        template<class COMPONENT>
        uint32_t componentCount() {
            return componentVector<COMPONENT>().size();
        }

        uint32_t entityCount() {
            return mDenseEntities.size();
        }

    private:
        std::vector<uint32_t> mSparseEntities{};
        std::vector<uint32_t> mDenseEntities{};
        std::vector<std::array<std::optional<uint32_t>, sizeof...(COMPONENTS)>> mIndexArrays{};

        std::map<std::type_index, uint32_t> mTypeIndexMap{};
        std::tuple<std::vector<COMPONENTS>...> mComponentVectors{};

        template<typename T>
        inline std::vector<T> &componentVector() {
            return std::get<std::vector<T>>(mComponentVectors);
        }

        template<class T, class T2, class... OTHER>
        void removeAllComponents(uint32_t entity) {
            removeComponent<T>(entity);

            removeAllComponents<T2, OTHER...>(entity);
        }

        template<class T>
        void removeAllComponents(uint32_t entity) {
            removeComponent<T>(entity);
        }

        template<class T, class T2, class... OTHER>
        void collectArchetypeMatches(std::vector<bool> &valid) {
            for (uint32_t i = 0; i < valid.size(); ++i) {
                if (valid[i] && !matchesArchetype<T>(i)) {
                    valid[i] = false;
                }
            }

            collectArchetypeMatches<T2, OTHER...>(valid);
        }

        template<class T>
        void collectArchetypeMatches(std::vector<bool> &valid) {
            for (uint32_t i = 0; i < valid.size(); ++i) {
                if (valid[i] && !matchesArchetype<T>(i)) {
                    valid[i] = false;
                }
            }
        }

        template<class T>
        inline bool matchesArchetype(uint32_t denseIndex) {
            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(T))];
            const std::optional<uint32_t> &componentIndex = mIndexArrays[denseIndex][typeIndex];

            return componentIndex.has_value();
        }

        template<class TUPLE, class T, class T2, class... OTHER>
        void resizeArchetypes(TUPLE &output, uint32_t totalMatches) {
            std::get<std::vector<T *>>(output).resize(totalMatches);

            resizeArchetypes<TUPLE, T2, OTHER...>(output, totalMatches);
        }

        template<class TUPLE, class T>
        void resizeArchetypes(TUPLE &output, uint32_t totalMatches) {
            std::get<std::vector<T *>>(output).resize(totalMatches);
        }

        template<class TUPLE, class T, class T2, class... OTHER>
        inline void insertArchetypeComponents(TUPLE &output, const std::array<std::optional<uint32_t>, sizeof...(COMPONENTS)> &indexArray, uint32_t archetypeIndex) {
            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(T))];
            const uint32_t componentIndex = *indexArray[typeIndex];

            std::get<std::vector<T *>>(output)[archetypeIndex] = &componentVector<T>()[componentIndex];

            insertArchetypeComponents<TUPLE, T2, OTHER...>(output, indexArray, archetypeIndex);
        }

        template<class TUPLE, class T>
        inline void insertArchetypeComponents(TUPLE &output, const std::array<std::optional<uint32_t>, sizeof...(COMPONENTS)> &indexArray, uint32_t archetypeIndex) {
            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(T))];
            const uint32_t componentIndex = *indexArray[typeIndex];

            std::get<std::vector<T *>>(output)[archetypeIndex] = &componentVector<T>()[componentIndex];
        }

        template<class T>
        inline void insertSingleArchetypeComponents(std::vector<T *> &output, const std::array<std::optional<uint32_t>, sizeof...(COMPONENTS)> &indexArray, uint32_t archetypeIndex) {
            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(T))];
            const uint32_t componentIndex = *indexArray[typeIndex];

            output[archetypeIndex] = &componentVector<T>()[componentIndex];
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
            Doughnut::Log::v("Adding type ", typeid(T).name());

            const auto typeIndex = std::type_index(typeid(T));
            assert(!mTypeIndexMap.contains(typeIndex) && "Can not add multiple components of the same type due to ambiguity");
            mTypeIndexMap[typeIndex] = mTypeIndexMap.size();
        }
    };

    void testEntityManager();

    void benchmark(uint32_t count);
}

#endif //DOUGHNUT_ENTITY_MANAGER_H
