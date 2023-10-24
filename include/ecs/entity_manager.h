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

// TODO DEADLOCKED SOMEWHERE HERE
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

        size_t makeEntity() {
            std::lock_guard<std::mutex> guard{mMakeEntityMutex};

            assert(mDenseEntities.size() == mIndexArrays.size());

            size_t newIndex;
            if (mSparseEntities.size() == mDenseEntities.size()) {
                newIndex = mSparseEntities.size();
                mSparseEntities.emplace_back();
            } else {
                for (size_t i = 0; i < mSparseEntities.size(); ++i) {
                    if (mSparseEntities[i] == std::numeric_limits<size_t>::max()) {
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

        void requestEntityDeletion(size_t entity) {
            std::lock_guard<std::mutex> guard{mDeleteEntityMutex};

            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            mEntitiesToRemove.push_back(entity);
        }

        template<class COMPONENT>
        void insertComponent(COMPONENT component, size_t entity) {
            const size_t typeIndex = mTypeIndexMap[std::type_index(typeid(COMPONENT))];
            std::lock_guard<std::mutex> guard{mInsertComponentMutices[typeIndex]};

            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            const size_t denseId = mSparseEntities[entity];

            if (!mIndexArrays[denseId][typeIndex].has_value()) {
                Doughnut::Log::v("Inserting component ", typeid(COMPONENT).name());
                mIndexArrays[denseId][typeIndex] = componentVector<COMPONENT>().size();
                componentVector<COMPONENT>().emplace_back(component);
            } else {
                Doughnut::Log::v("Overriding component ", typeid(COMPONENT).name());
                size_t componentId = *mIndexArrays[denseId][typeIndex];
                componentVector<COMPONENT>()[componentId] = std::move(component);
            }
        }

        template<class COMPONENT>
        void insertComponent(size_t entity) {
            const size_t typeIndex = mTypeIndexMap[std::type_index(typeid(COMPONENT))];
            std::lock_guard<std::mutex> guard{mInsertComponentMutices[typeIndex]};

            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            const size_t denseId = mSparseEntities[entity];

            if (!mIndexArrays[denseId][typeIndex].has_value()) {
                Doughnut::Log::v("Inserting component ", typeid(COMPONENT).name());
                mIndexArrays[denseId][typeIndex] = componentVector<COMPONENT>().size();
                componentVector<COMPONENT>().emplace_back();
            } else {
                Doughnut::Log::v("Overriding component ", typeid(COMPONENT).name());
                size_t componentId = *mIndexArrays[denseId][typeIndex];
                componentVector<COMPONENT>().emplace(componentVector<COMPONENT>().begin() + componentId);
            }
        }

        template<class COMPONENT>
        void requestComponentDeletion(size_t entity) {
            const size_t typeIndex = mTypeIndexMap[std::type_index(typeid(COMPONENT))];
            std::lock_guard<std::mutex> guard{mDeleteComponentMutices[typeIndex]};

            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            mDeletableComponentVectors[typeIndex].push_back(entity);
        }

        void commitDeletions() {
            std::lock_guard<std::mutex> guard1{mDeleteEntityMutex};
            std::lock_guard<std::mutex> guard2{mMakeEntityMutex};

            for (const auto entity: mEntitiesToRemove) {
                assert(entity < mSparseEntities.size()
                       && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                       && "Accessing non-existent entity.");

                assert(mDenseEntities.size() == mIndexArrays.size());
                assert(entity < mSparseEntities.size());

                size_t denseIndex = mSparseEntities[entity];
                assert(denseIndex < mDenseEntities.size());

                deleteAllComponents<COMPONENTS...>(entity);

                size_t otherSparseIndex = mDenseEntities.back();
                mSparseEntities[otherSparseIndex] = denseIndex;
                mDenseEntities[denseIndex] = otherSparseIndex;
                mDenseEntities.pop_back();
                mIndexArrays[denseIndex] = mIndexArrays.back();
                mIndexArrays.pop_back();

                mSparseEntities[entity] = std::numeric_limits<size_t>::max();
            }

            deleteDeletableComponents<COMPONENTS...>();
        }

        template<class COMPONENT>
        COMPONENT *getComponent(size_t entity) {
            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            const size_t denseIndex = mSparseEntities[entity];
            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(COMPONENT))];
            const std::optional<size_t> &componentIndex = mIndexArrays[denseIndex][typeIndex];

            assert(componentIndex.has_value());

            return &componentVector<COMPONENT>()[*componentIndex];
        }

        // TODO sort for better cache hits
        template<class T, class... OTHER>
        std::conditional<(sizeof...(OTHER) > 0),
                std::vector<std::tuple<T *, OTHER *...>>,
                std::vector<T *>
        >::type getArchetype() {
            typename std::conditional<(sizeof...(OTHER) > 0),
                    std::vector<std::tuple<T *, OTHER *...>>,
                    std::vector<T *>
            >::type out{};

            std::vector<bool> matchesArchetype{};
            matchesArchetype.resize(mDenseEntities.size(), true);

            collectArchetypeMatches<T, OTHER...>(matchesArchetype);

            size_t totalMatches = 0;
            for (auto b: matchesArchetype) {
                if (b) ++totalMatches;
            }

            out.resize(totalMatches);

            size_t archetypeIndex = 0;
            for (size_t denseIndex = 0; denseIndex < mDenseEntities.size(); ++denseIndex) {
                if (matchesArchetype[denseIndex]) {

                    if constexpr (sizeof...(OTHER) > 0) {
                        insertArchetypeComponents<std::tuple<T *, OTHER *...>, T, OTHER...>(out, mIndexArrays[denseIndex], archetypeIndex);
                    } else {
                        insertSingleArchetypeComponents<T>(out, mIndexArrays[denseIndex], archetypeIndex);
                    }

                    ++archetypeIndex;
                }
            }

            return out;
        }

        template<class COMPONENT>
        size_t componentCount() {
            return componentVector<COMPONENT>().size();
        }

        size_t entityCount() {
            return mDenseEntities.size();
        }

    private:
        std::vector<size_t> mSparseEntities{};
        std::vector<size_t> mDenseEntities{};
        std::vector<std::array<std::optional<size_t>, sizeof...(COMPONENTS)>> mIndexArrays{};

        std::map<std::type_index, size_t> mTypeIndexMap{};
        std::tuple<std::vector<COMPONENTS>...> mComponentVectors{};

        std::mutex mMakeEntityMutex{};
        std::mutex mDeleteEntityMutex{};
        std::vector<size_t> mEntitiesToRemove{};

        std::array<std::mutex, sizeof...(COMPONENTS)> mInsertComponentMutices{};
        std::array<std::mutex, sizeof...(COMPONENTS)> mDeleteComponentMutices{};
        std::array<std::vector<size_t>, sizeof...(COMPONENTS)> mDeletableComponentVectors{};

        template<typename T>
        inline std::vector<T> &componentVector() {
            return std::get<std::vector<T>>(mComponentVectors);
        }

        template<class COMPONENT>
        void deleteComponent(size_t entity) {
            const size_t typeIndex = mTypeIndexMap[std::type_index(typeid(COMPONENT))];
            std::lock_guard<std::mutex> guard{mDeleteComponentMutices[typeIndex]};

            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            assert(mDenseEntities.size() == mIndexArrays.size());
            assert(entity < mSparseEntities.size());

            const size_t denseIndex = mSparseEntities[entity];
            assert(denseIndex < mDenseEntities.size());

            std::optional<size_t> &componentIndex = mIndexArrays[denseIndex][typeIndex];
            if (componentIndex.has_value()) {
                const auto currentLastIndex = componentVector<COMPONENT>().size() - 1;

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

        template<class T, class... OTHER>
        void deleteAllComponents(size_t entity) {
// TODO            (requestComponentDeletion<OTHER>(entity), ...);

            deleteComponent<T>(entity);

            if constexpr (sizeof...(OTHER) > 0)
                deleteAllComponents<OTHER...>(entity);
        }

        template<class T, class... OTHER>
        void deleteDeletableComponents() {
            const size_t typeIndex = mTypeIndexMap[std::type_index(typeid(T))];
            auto &deletableVector = mDeletableComponentVectors[typeIndex];

            for (const size_t entity: deletableVector) {
                deleteComponent<T>(entity);
            }

            deletableVector.clear();

            if constexpr (sizeof...(OTHER) > 0)
                deleteDeletableComponents<OTHER...>();
        }

        template<class T, class... OTHER>
        void collectArchetypeMatches(std::vector<bool> &valid) {
            for (size_t i = 0; i < valid.size(); ++i) {
                if (valid[i] && !matchesArchetype<T>(i)) {
                    valid[i] = false;
                }
            }

            if constexpr (sizeof...(OTHER) > 0)
                collectArchetypeMatches<OTHER...>(valid);
        }

        template<class T>
        inline bool matchesArchetype(size_t denseIndex) {
            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(T))];
            const std::optional<size_t> &componentIndex = mIndexArrays[denseIndex][typeIndex];

            return componentIndex.has_value();
        }

        template<class TUPLE, class T, class... OTHER>
        void resizeArchetypes(TUPLE &output, size_t totalMatches) {
            std::get<std::vector<T *>>(output).resize(totalMatches);

            if constexpr (sizeof...(OTHER) > 0)
                resizeArchetypes<TUPLE, OTHER...>(output, totalMatches);
        }

        template<class TUPLE, class T, class... OTHER>
        inline void insertArchetypeComponents(std::vector<TUPLE> &output, const std::array<std::optional<size_t>, sizeof...(COMPONENTS)> &indexArray, size_t archetypeIndex) {
            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(T))];
            const size_t componentIndex = *indexArray[typeIndex];

            std::get<T *>(output[archetypeIndex]) = &componentVector<T>()[componentIndex];

            if constexpr (sizeof...(OTHER) > 0)
                insertArchetypeComponents<TUPLE, OTHER...>(output, indexArray, archetypeIndex);
        }

        template<class T>
        inline void insertSingleArchetypeComponents(std::vector<T *> &output, const std::array<std::optional<size_t>, sizeof...(COMPONENTS)> &indexArray, size_t archetypeIndex) {
            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(T))];
            const size_t componentIndex = *indexArray[typeIndex];

            output[archetypeIndex] = &componentVector<T>()[componentIndex];
        }

        template<class T, class... OTHER>
        void init() {
            Doughnut::Log::v("Adding type ", typeid(T).name());

            const auto typeIndex = std::type_index(typeid(T));
            assert(!mTypeIndexMap.contains(typeIndex) && "Can not add multiple components of the same type due to ambiguity");
            mTypeIndexMap[typeIndex] = mTypeIndexMap.size();

            if constexpr (sizeof...(OTHER) > 0)
                init<OTHER...>();
        }
    };

    void testEntityManager();

    void benchmark(size_t count);
}

#endif //DOUGHNUT_ENTITY_MANAGER_H
