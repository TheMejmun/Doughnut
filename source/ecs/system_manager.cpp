//
// Created by Sam on 2023-10-08.
//

#include "ecs/system_manager.h"

using namespace dn;

// TODO could fail due to race conditions in 'TestSystem::update'
void dn::testSystemManager() {
    EntityManager<int, long, std::mutex *> em{};

    em.makeEntity();
    em.insertComponent<int>(0);
    em.insertComponent<long>(0);
    std::mutex updateDataMutex{};
    em.insertComponent<std::mutex *>(&updateDataMutex, 0);
    auto &executions = *(em.getArchetype<int>()[0].components);
    auto &constructed = *(em.getArchetype<long>()[0].components);

    SystemManager<decltype(em), 3> sm{&em};

    class TestSystem : public System<decltype(em)> {
    public:
        ~TestSystem() override = default;

        void update(const double delta, decltype(em) &entityManager) override {
            auto &executions = *entityManager.getArchetype<int>()[0].components;
            auto &constructed = *entityManager.getArchetype<long>()[0].components;
            auto &mutex = **entityManager.getArchetype<std::mutex *>()[0].components;
            std::lock_guard<std::mutex> guard{mutex};
            if (firstUpdate) {
                constructed += 1;
                firstUpdate = false;
            }
            executions += 1;
        }

    private:
        bool firstUpdate = true;
    };

    assert(constructed == 0);
    assert(executions == 0);

    sm.insertSystem<TestSystem, 0>();
    sm.insertSystem<TestSystem, 0>();
    sm.insertSystem<TestSystem, 1>();
    sm.insertSystem<TestSystem, 1>();
    sm.insertSystem<TestSystem, 2>();
    sm.insertSystem<TestSystem, 2>();

    sm.update(0);

    assert(constructed == 6);
    assert(executions == 6);

    sm.update(0);

    assert(constructed == 6);
    assert(executions == 12);

    dn::log::i("SystemManager test successful.");
}