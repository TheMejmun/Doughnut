//
// Created by Sam on 2023-09-27.
//

#ifndef DOUGHNUT_ENTITY_MANAGER_H
#define DOUGHNUT_ENTITY_MANAGER_H

#include "io/logger.h"
#include "util/timer.h"
#include "util/templates.h"
#include "core/types.h"

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
    // TODO add this to components
    struct EntityInfo {
        size_t entity;
    };

    template<class... COMPONENTS>
    class EntityManager {
    public:
        EntityManager() {
            Doughnut::Log::i("Creating EntityManager");

            ([&] {
                Doughnut::Log::v("Adding type", typeid(COMPONENTS).name());

                const auto typeIndex = std::type_index(typeid(COMPONENTS));
                assert(!mTypeIndexMap.contains(typeIndex) && "Can not add multiple components of the same type due to ambiguity");
                mTypeIndexMap[typeIndex] = mTypeIndexMap.size();
            }(), ...);
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
            const size_t typeIndex = indexOf<COMPONENT>();
            std::lock_guard<std::mutex> guard{mInsertComponentMutices[typeIndex]};

            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            const size_t denseId = mSparseEntities[entity];

            if (!mIndexArrays[denseId][typeIndex].has_value()) {
                mIndexArrays[denseId][typeIndex] = componentVector<COMPONENT>().size();
                componentVector<COMPONENT>().emplace_back(entity, component);
            } else {
                size_t componentId = *mIndexArrays[denseId][typeIndex];
                componentVector<COMPONENT>()[componentId] = {entity, component};
            }
        }

        template<class COMPONENT>
        void insertComponent(size_t entity) {
            const size_t typeIndex = indexOf<COMPONENT>();
            std::lock_guard<std::mutex> guard{mInsertComponentMutices[typeIndex]};

            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            const size_t denseId = mSparseEntities[entity];

            if (!mIndexArrays[denseId][typeIndex].has_value()) {
                mIndexArrays[denseId][typeIndex] = componentVector<COMPONENT>().size();
                componentVector<COMPONENT>().emplace_back(entity, COMPONENT());
            } else {
                size_t componentId = *mIndexArrays[denseId][typeIndex];
                componentVector<COMPONENT>()[componentId] = {entity, COMPONENT()};
            }
        }

        template<class COMPONENT>
        void requestComponentDeletion(size_t entity) {
            const size_t typeIndex = indexOf<COMPONENT>();
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

                deleteAllComponents(entity);

                size_t otherSparseIndex = mDenseEntities.back();
                mSparseEntities[otherSparseIndex] = denseIndex;
                mDenseEntities[denseIndex] = otherSparseIndex;
                mDenseEntities.pop_back();
                mIndexArrays[denseIndex] = mIndexArrays.back();
                mIndexArrays.pop_back();

                mSparseEntities[entity] = std::numeric_limits<size_t>::max();
            }

            deleteDeletableComponents();
        }

        template<class COMPONENT>
        COMPONENT *getComponent(size_t entity) {
            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            const size_t denseIndex = mSparseEntities[entity];
            const std::optional<size_t> &componentIndex = mIndexArrays[denseIndex][indexOf<COMPONENT>()];

            assert(componentIndex.has_value());

            return &componentVector<COMPONENT>()[*componentIndex].value;
        }

        // TODO sort for better cache hits
        template<class... T>
        std::vector<typename Doughnut::TupleOrSingle<T *...>::Type> getArchetype() {
            typename std::vector<typename Doughnut::TupleOrSingle<T *...>::Type> out{};

            std::vector<bool> matchesArchetype{};
            matchesArchetype.resize(mDenseEntities.size(), true);

            collectArchetypeMatches<T...>(matchesArchetype);

            size_t totalMatches = 0;
            for (auto b: matchesArchetype) {
                if (b) ++totalMatches;
            }
            Doughnut::Log::v("Archetype matches:", totalMatches, "for", typeid(typename Doughnut::TupleOrSingle<T...>::Type).name());

            out.resize(totalMatches);

            size_t archetypeIndex = 0;
            for (size_t denseIndex = 0; denseIndex < mDenseEntities.size(); ++denseIndex) {
                if (matchesArchetype[denseIndex]) {

                    if constexpr (sizeof...(T) > 1) {
                        insertArchetypeComponents<T...>(out, mIndexArrays[denseIndex], archetypeIndex);
                    } else {
                        insertSingleArchetypeComponents<typename Doughnut::FirstOf<T...>::Type>(out, mIndexArrays[denseIndex], archetypeIndex);
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
        std::tuple<std::vector<Doughnut::WithIndex<COMPONENTS>>...> mComponentVectors{};

        std::mutex mMakeEntityMutex{};
        std::mutex mDeleteEntityMutex{};
        std::vector<size_t> mEntitiesToRemove{};

        std::array<std::mutex, sizeof...(COMPONENTS)> mInsertComponentMutices{};
        std::array<std::mutex, sizeof...(COMPONENTS)> mDeleteComponentMutices{};
        std::array<std::vector<size_t>, sizeof...(COMPONENTS)> mDeletableComponentVectors{};

        template<typename COMPONENT>
        inline std::vector<Doughnut::WithIndex<COMPONENT>> &componentVector() {
            return std::get<std::vector<Doughnut::WithIndex<COMPONENT>>>(mComponentVectors);
        }

        template<class COMPONENT>
        inline size_t indexOf() {
            return mTypeIndexMap[std::type_index(typeid(COMPONENT))];
        }

        template<class COMPONENT>
        void deleteComponent(size_t entity) {
            const size_t typeIndex = indexOf<COMPONENT>();
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

                // Move current rear to deleted position and update other entity with new component position
                componentVector<COMPONENT>()[*componentIndex] = componentVector<COMPONENT>().back();

                const size_t otherEntity = componentVector<COMPONENT>().back().index;
                const size_t otherDenseIndex = mSparseEntities[otherEntity];
                mIndexArrays[otherDenseIndex][typeIndex] = *componentIndex;

                componentVector<COMPONENT>().pop_back();
                componentIndex.reset();
            }
        }

        void deleteAllComponents(size_t entity) {
            (deleteComponent<COMPONENTS>(entity), ...);
        }

        void deleteDeletableComponents() {
            ([&] {
                auto &deletableVector = mDeletableComponentVectors[indexOf<COMPONENTS>()];

                for (const size_t entity: deletableVector) {
                    deleteComponent<COMPONENTS>(entity);
                }

                deletableVector.clear();
            }(), ...);
        }

        template<class... T>
        void collectArchetypeMatches(std::vector<bool> &valid) {
            ([&] {
                for (size_t i = 0; i < valid.size(); ++i) {
                    if (valid[i] && !hasComponent<T>(i)) {
                        valid[i] = false;
                    }
                }
            }(), ...);
        }

        template<class COMPONENT>
        bool hasComponent(size_t denseIndex) {
            const std::optional<size_t> &componentIndex = mIndexArrays[denseIndex][indexOf<COMPONENT>()];

            return componentIndex.has_value();
        }

        template<class... T>
        void insertArchetypeComponents(std::vector<std::tuple<T *...>> &output, const std::array<std::optional<size_t>, sizeof...(COMPONENTS)> &indexArray, size_t archetypeIndex) {
            ([&] {
                const size_t componentIndex = *indexArray[indexOf<T>()];

                std::get<T *>(output[archetypeIndex]) = &componentVector<T>()[componentIndex].value;
            }(), ...);
        }

        template<class COMPONENT>
        void insertSingleArchetypeComponents(std::vector<COMPONENT *> &output, const std::array<std::optional<size_t>, sizeof...(COMPONENTS)> &indexArray, size_t archetypeIndex) {
            const size_t componentIndex = *indexArray[indexOf<COMPONENT>()];

            output[archetypeIndex] = &componentVector<COMPONENT>()[componentIndex].value;
        }
    };

    void testEntityManager();

    void benchmark(size_t count);
}

#endif //DOUGHNUT_ENTITY_MANAGER_H
