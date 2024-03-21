//
// Created by Sam on 2023-09-27.
//

#ifndef DOUGHNUT_ENTITY_MANAGER_H
#define DOUGHNUT_ENTITY_MANAGER_H

#include "io/logger.h"
#include "util/timer.h"
#include "util/templates.h"
#include "core/with_index.h"

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

namespace dn {
    template<class... COMPONENTS>
    struct Reference {
        size_t entity;
        typename dn::TupleOrSingle<COMPONENTS *...>::Type components;

        template<class COMPONENT>
        inline COMPONENT *get() const {
            if constexpr (sizeof...(COMPONENTS)) {
                return std::get<COMPONENT *>(components);
            } else {
                return components;
            }
        }
    };

    template<class... COMPONENTS>
    class EntityManager {
    public:
        EntityManager() {
            dn::log::d("Creating EntityManager");

            ([&] {
                dn::log::v("Adding type", typeid(COMPONENTS).name());

                const auto typeIndex = std::type_index(typeid(COMPONENTS));
                assert(!mTypeIndexMap.contains(typeIndex) && "Can not add multiple components of the same type due to ambiguity");
                mTypeIndexMap[typeIndex] = mTypeIndexMap.size();
            }(), ...);
        }

        ~EntityManager() {
            dn::log::d("Destroying EntityManager");
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

        void requestEntityDeletion(const size_t entity) {
            std::lock_guard<std::mutex> guard{mDeleteEntityMutex};

            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            mEntitiesToRemove.push_back(entity);
        }

        template<class COMPONENT>
        void insertComponent(COMPONENT component, const size_t entity) {
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
        void insertComponent(const size_t entity) {
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
        void requestComponentDeletion(const size_t entity) {
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
        COMPONENT *getComponent(const size_t entity) {
            assert(entity < mSparseEntities.size()
                   && mSparseEntities[entity] != std::numeric_limits<size_t>::max()
                   && "Accessing non-existent entity.");

            const size_t denseIndex = mSparseEntities[entity];
            const std::optional<size_t> &componentIndex = mIndexArrays[denseIndex][indexOf<COMPONENT>()];

            assert(componentIndex.has_value());

            return &componentVector<COMPONENT>()[*componentIndex].value;
        }

        // TODO sort for better cache hits
        template<class... COMPONENT>
        std::vector<Reference<COMPONENT ...>> getArchetype() {
            typename std::vector<Reference<COMPONENT ...>> out{};

            std::vector<bool> matchesArchetype{};
            matchesArchetype.resize(mDenseEntities.size(), true);

            collectArchetypeMatches<COMPONENT...>(matchesArchetype);

            size_t totalMatches = 0;
            for (auto b: matchesArchetype) {
                if (b) ++totalMatches;
            }
            dn::log::v("Archetype matches:", totalMatches, "for", typeid(typename dn::TupleOrSingle<COMPONENT...>::Type).name());

            out.resize(totalMatches);

            size_t archetypeIndex = 0;
            for (size_t denseIndex = 0; denseIndex < mDenseEntities.size(); ++denseIndex) {
                if (matchesArchetype[denseIndex]) {

                    out[archetypeIndex].entity = mDenseEntities[denseIndex];

                    if constexpr (sizeof...(COMPONENT) > 1) {
                        insertArchetypeComponents<COMPONENT...>(out, mIndexArrays[denseIndex], archetypeIndex);
                    } else {
                        insertSingleArchetypeComponents<typename dn::FirstOf<COMPONENT...>::Type>(out, mIndexArrays[denseIndex], archetypeIndex);
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
        std::tuple<std::vector<dn::WithIndex<COMPONENTS>>...> mComponentVectors{};

        std::mutex mMakeEntityMutex{};
        std::mutex mDeleteEntityMutex{};
        std::vector<size_t> mEntitiesToRemove{};

        std::array<std::mutex, sizeof...(COMPONENTS)> mInsertComponentMutices{};
        std::array<std::mutex, sizeof...(COMPONENTS)> mDeleteComponentMutices{};
        std::array<std::vector<size_t>, sizeof...(COMPONENTS)> mDeletableComponentVectors{};

        template<typename T>
        inline std::vector<dn::WithIndex<T>> &componentVector() {
            return std::get<std::vector<dn::WithIndex<T>>>(mComponentVectors);
        }

        template<class COMPONENT>
        inline size_t indexOf() {
            return mTypeIndexMap[std::type_index(typeid(COMPONENT))];
        }

        template<class COMPONENT>
        void deleteComponent(const size_t entity) {
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

        void deleteAllComponents(const size_t entity) {
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

        template<class T>
        bool hasComponent(const size_t denseIndex) {
            const std::optional<size_t> &componentIndex = mIndexArrays[denseIndex][indexOf<T>()];

            return componentIndex.has_value();
        }

        template<class... T>
        void insertArchetypeComponents(std::vector<Reference<T...>> &output, const std::array<std::optional<size_t>, sizeof...(COMPONENTS)> &indexArray, const size_t archetypeIndex) {
            ([&] {
                const size_t componentIndex = *indexArray[indexOf<T>()];

                std::get<T *>(output[archetypeIndex].components) = &componentVector<T>()[componentIndex].value;
            }(), ...);
        }

        template<class T>
        void insertSingleArchetypeComponents(std::vector<Reference<T>> &output, const std::array<std::optional<size_t>, sizeof...(COMPONENTS)> &indexArray, const size_t archetypeIndex) {
            const size_t componentIndex = *indexArray[indexOf<T>()];

            output[archetypeIndex].components = &componentVector<T>()[componentIndex].value;
        }
    };

    void testEntityManager();

    void benchmarkEntityManager(size_t count);
}

#endif //DOUGHNUT_ENTITY_MANAGER_H
