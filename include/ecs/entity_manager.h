//
// Created by Sam on 2023-09-27.
//

#ifndef DOUGHNUT_ENTITY_MANAGER_H
#define DOUGHNUT_ENTITY_MANAGER_H

#include "io/printer.h"
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
            info("Creating EntityManager");
            init<COMPONENTS...>();
        }

        ~EntityManager() {
            info("Destroying EntityManager");
        }

        uint32_t makeEntity() {
            assert(mDenseEntities.size() == mIndexArrays.size());

            uint32_t newIndex;

            if(mSparseEntities.size() == mDenseEntities.size()){
                newIndex = mSparseEntities.size();
                mSparseEntities.emplace_back();
            }else {
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
                verbose("Inserting component " << typeid(COMPONENT).name());
                mIndexArrays[denseId][componentVectorId] = componentVector<COMPONENT>().size();
                componentVector<COMPONENT>().emplace_back(component);
            } else {
                verbose("Overriding component " << typeid(COMPONENT).name());
                uint32_t componentId = *mIndexArrays[denseId][componentVectorId];
                componentVector<COMPONENT>()[componentId] = std::move(component);
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


        template<class T, class... OTHER>
        std::tuple<std::vector<T *>, std::vector<OTHER *>...> requestAll() {
            std::tuple<std::vector<T *>, std::vector<OTHER *>...> out{};

            std::vector<bool> matchesArchetype{};
            matchesArchetype.resize(mDenseEntities.size(), true);

            collectArchetypeMatches<T, OTHER...>(matchesArchetype);

            uint32_t totalMatches = 0;
            for (auto b: matchesArchetype) {
                if (b) ++totalMatches;
            }

            resizeArchetypes<std::tuple<std::vector<T *>, std::vector<OTHER *>...>, T, OTHER...>(out, totalMatches);

            uint32_t archetypeIndex = 0;
            for (uint32_t denseIndex = 0; denseIndex < mDenseEntities.size(); ++denseIndex) {
                if (matchesArchetype[denseIndex]) {
                    insertArchetypeComponents<std::tuple<std::vector<T *>, std::vector<OTHER *>...>, T, OTHER...>(out, mIndexArrays[denseIndex], archetypeIndex);
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

        ECS2::EntityManager<test, int, double, uint32_t> em;
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

        auto id3 = em.makeEntity();
        assert(em.entityCount() == 3);
        assert(em.componentCount<int>() == 1);

        em.insertComponent(3, id2);
        assert(em.entityCount() == 3);
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
        assert(em.entityCount() == 2);
        assert(em.componentCount<int>() == 1);
        assert(em.getComponent<int>(id2) == 3);

        std::cout << "EntityManager test successful." << std::endl;
    }

    void benchmark(uint32_t count) {
        struct Component1 {
            double one;
            double two;
            double three;
            double four;
        };
        struct Component2 {
            double one;
            double two;
            double three;
            double four;
        };
        struct Component3 {
            double one;
            double two;
            double three;
            double four;
        };
        struct Component4 {
            double one;
            double two;
            double three;
            double four;
        };
        struct Component5 {
            double one;
            double two;
            double three;
            double four;
        };
        struct Component6 {
            double one;
            double two;
            double three;
            double four;
        };
        struct Component7 {
            double one;
            double two;
            double three;
            double four;
        };
        struct Component8 {
            double one;
            double two;
            double three;
            double four;
        };

        EntityManager<Component1, Component2, Component3, Component4, Component5, Component6, Component7, Component8> em{};

        {
            auto _trace_before = Doughnut::Timer::now();
            for (uint32_t i = 0; i < count; ++i) {
                em.makeEntity();
            }
            auto _trace_after = Doughnut::Timer::now();
            auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
            std::cout << "MAKE ENTITIES" << ": " << _duration << std::endl;
        }

        {
            auto _trace_before = Doughnut::Timer::now();
            for (uint32_t i = 0; i < count; ++i) {
                em.insertComponent(Component1{}, i);
            }
            auto _trace_after = Doughnut::Timer::now();
            auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
            std::cout << "INSERT C1" << ": " << _duration << std::endl;
        }

        {
            auto _trace_before = Doughnut::Timer::now();
            for (uint32_t i = 0; i < count; ++i) {
                if (i % 2 == 0) {
                    em.insertComponent(Component2{}, i);
                }
            }
            auto _trace_after = Doughnut::Timer::now();
            auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
            std::cout << "INSERT C2" << ": " << _duration << std::endl;
        }

        {
            auto _trace_before = Doughnut::Timer::now();
            for (uint32_t i = 0; i < count; ++i) {
                if (i % 3 == 0) {
                    em.insertComponent(Component3{}, i);
                }
            }
            auto _trace_after = Doughnut::Timer::now();
            auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
            std::cout << "INSERT C3" << ": " << _duration << std::endl;
        }

        {
            auto _trace_before = Doughnut::Timer::now();
            auto requested = em.requestAll<Component1>();
            auto _trace_after = Doughnut::Timer::now();
            auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
            std::cout << "GET ARCHETYPE 0" << ": " << _duration << std::endl;
        }

        {
            auto _trace_before = Doughnut::Timer::now();
            auto requested = em.requestAll<Component1, Component2>();
            auto _trace_after = Doughnut::Timer::now();
            auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
            std::cout << "GET ARCHETYPE 1" << ": " << _duration << std::endl;
        }

        {
            auto _trace_before = Doughnut::Timer::now();
            auto requested = em.requestAll<Component1, Component2, Component3>();
            auto _trace_after = Doughnut::Timer::now();
            auto _duration = std::to_string(Doughnut::Timer::duration(_trace_before, _trace_after));
            std::cout << "GET ARCHETYPE 2" << ": " << _duration << std::endl;
        }
    }
}

#endif //DOUGHNUT_ENTITY_MANAGER_H
