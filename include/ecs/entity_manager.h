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

        template<class COMPONENT>
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

        template<class COMPONENT>
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

        template<class COMPONENT>
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


        template<class T, class... OTHER>
        std::tuple<std::vector<T *>, std::vector<OTHER *>...> requestAll() {
            std::tuple<std::vector<T *>, std::vector<OTHER *>...> out{};

            std::vector<bool> matchesArchetype{};
            matchesArchetype.resize(mDenseEntities.size(), true);

            collectArchetypeMatches<T, OTHER...>(matchesArchetype);

            size_t totalMatches = 0;
            for (auto b: matchesArchetype) {
                if (b) ++totalMatches;
            }

            resizeTuple<std::tuple<std::vector<T *>, std::vector<OTHER *>...>, T *, OTHER *...>(out, totalMatches);

            for (size_t denseIndex = 0; denseIndex < mDenseEntities.size(); ++denseIndex) {
                if (matchesArchetype[denseIndex]) {
                    collectArchetypeComponents<std::tuple<std::vector<T *>, std::vector<OTHER *>...>, T, OTHER...>(out, matchesArchetype);
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
        void collectArchetypeMatches(std::vector<bool> &valid) {
            for (size_t i = 0; i < valid.size(); ++i) {
                if (!matchesArchetype<T>(i)) {
                    valid[i] = false;
                }
            }

            collectArchetypeMatches<T2, OTHER...>(valid);
        }

        template<class T>
        void collectArchetypeMatches(std::vector<bool> &valid) {
            for (size_t i = 0; i < valid.size(); ++i) {
                if (!matchesArchetype<T>(i)) {
                    valid[i] = false;
                }
            }
        }

        template<class T>
        inline bool matchesArchetype(size_t denseIndex) {
            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(T))];
            const std::optional<size_t> &componentIndex = mIndexVectors[typeIndex][denseIndex];

            return componentIndex.has_value();
        }

        template<class TUPLE, class T, class T2, class... OTHER>
        void collectArchetypeComponents(TUPLE &output, const std::vector<bool> &matches) {
            auto &cRefs = std::get<std::vector<T *>>(output);

            for (size_t denseIndex = 0; denseIndex < mDenseEntities.size(); ++denseIndex) {
                if (matches[denseIndex]) {
                    insertArchetypeComponents(cRefs, denseIndex);
                }
            }

            collectArchetypeComponents<TUPLE, T2, OTHER...>(output, matches);
        }

        template<class TUPLE, class T>
        void collectArchetypeComponents(TUPLE &output, const std::vector<bool> &matches) {
            auto &cRefs = std::get<std::vector<T *>>(output);

            for (size_t denseIndex = 0; denseIndex < mDenseEntities.size(); ++denseIndex) {
                if (matches[denseIndex]) {
                    insertArchetypeComponents(cRefs, denseIndex);
                }
            }
        }

        template<class T>
        inline void insertArchetypeComponents(std::vector<T *> &cRefs, size_t index) {
            const auto typeIndex = mTypeIndexMap[std::type_index(typeid(T))];
            const size_t componentIndex = mIndexVectors[typeIndex][index].value();

            cRefs[index] = &componentVector<T>()[componentIndex];
        }

        template<class TUPLE, class T, class T2, class... OTHER>
        void resizeTuple(TUPLE &output, size_t size) {
            std::get<std::vector<T>>(output).resize(size);

            resizeTuple<TUPLE, T2, OTHER...>(output, size);
        }

        template<class TUPLE, class T>
        void resizeTuple(TUPLE &output, size_t size) {
            std::get<std::vector<T>>(output).resize(size);
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

    void testEntityManager() {
        struct test {
            uint32_t someValue = 0;
        };

        ECS::EntityManager<test, int, double, uint32_t> em;
        assert(em.entityCount() == 0);
        assert(em.componentCount<int>() == 0);

        auto id = em.makeEntity();
        assert(em.entityCount() == 1);
        assert(em.componentCount<int>() == 0);

        em.insertComponent(0, id);
        assert(em.entityCount() == 1);
        assert(em.componentCount<int>() == 1);
        assert(em.getComponent<int>(id) == 0);

        em.insertComponent(1, id);
        assert(em.entityCount() == 1);
        assert(em.componentCount<int>() == 1);
        assert(em.getComponent<int>(id) == 1);

        em.removeComponent<int>(id);
        assert(em.entityCount() == 1);
        assert(em.componentCount<int>() == 0);

        em.insertComponent(2, id);
        assert(em.entityCount() == 1);
        assert(em.componentCount<int>() == 1);
        assert(em.getComponent<int>(id) == 2);

        auto id2 = em.makeEntity();
        assert(em.entityCount() == 2);
        assert(em.componentCount<int>() == 1);

        em.insertComponent(3, id2);
        assert(em.entityCount() == 2);
        assert(em.componentCount<int>() == 2);
        assert(em.getComponent<int>(id2) == 3);
        assert(em.getComponent<int>(id) == 2);

        auto allInts = em.requestAll<int>();
        assert(std::get<0>(allInts).size() == 2);
        // Test values
        assert((*std::get<0>(allInts)[0] == 2 && *std::get<0>(allInts)[1] == 3) || (*std::get<0>(allInts)[0] == 3 && *std::get<0>(allInts)[1] == 2));

        auto allDoubles = em.requestAll<double>();
        assert(std::get<0>(allDoubles).empty());

        auto allIntsAndDoubles = em.requestAll<int, double>();
        assert(std::get<0>(allIntsAndDoubles).empty());
        assert(std::get<1>(allIntsAndDoubles).empty());

        em.removeEntity(id);
        assert(em.entityCount() == 1);
        assert(em.componentCount<int>() == 1);
        assert(em.getComponent<int>(id2) == 3);

        std::cout << "EntityManager test successful." << std::endl;
    }
}

#endif //DOUGHNUT_ENTITY_MANAGER_H