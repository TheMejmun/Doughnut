//
// Created by Sam on 2023-10-08.
//

#include "ecs/system_manager.h"

using namespace ECS2;

// TODO could fail due to race conditions in 'TestSystem::update'
void ECS2::testSystemManager() {
    EntityManager<int, long> em{};

    em.makeEntity();
    em.insertComponent<int>(0);
    em.insertComponent<long>(0);
    auto &executions = *em.requestAll<int>()[0];
    auto &constructed = *em.requestAll<long>()[0];

    SystemManager<decltype(em), 3> sm{&em};

    class TestSystem : public System<decltype(em)> {
    public:
        ~TestSystem() override = default;

        void update(const double delta, decltype(em) &entityManager) override {
            auto &executions = *entityManager.requestAll<int>()[0];
            auto &constructed = *entityManager.requestAll<long>()[0];
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

    std::cout << "SystemManager test successful." << std::endl;
}