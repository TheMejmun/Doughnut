//
// Created by Sam on 2024-03-28.
//

#include "test_util.h"
#include "ecs/entity_manager.h"

struct test {
    size_t someValue = 0;
};

REGISTER(EntityManager) {
    TEST(testEntityManagerFlow) {
                                    dn::EntityManager<test, int, double> em;
                                    expect(em.entityCount() == 0
                                           && (em.componentCount<test>() == 0
                                               && em.componentCount<int>() == 0
                                               && em.componentCount<double>() == 0), "New EntityManager should be empty");

                                    auto id1 = em.makeEntity();
                                    expect(em.entityCount() == 1, "EntityManager should contain one entity after creating the first one");
                                    expect(em.componentCount<test>() == 0
                                           && em.componentCount<int>() == 0
                                           && em.componentCount<double>() == 0, "EntityManager should not have any components after just adding an entity");

                                    em.insertComponent(100, id1);
                                    expect(em.componentCount<test>() == 0
                                           && em.componentCount<int>() == 1
                                           && em.componentCount<double>() == 0, "EntityManager should have only one component after insertion");
                                    expect(*em.getComponent<int>(id1) == 100, "Inserted component should have the correct value");

                                    em.insertComponent(1, id1);
                                    expect(em.componentCount<test>() == 0
                                           && em.componentCount<int>() == 1
                                           && em.componentCount<double>() == 0, "EntityManager should still contain just one component after overriding it");
                                    expect(*em.getComponent<int>(id1) == 1, "Inserted component should override the old one");

                                    em.requestComponentDeletion<int>(id1);
                                    expect(em.entityCount() == 1
                                           && (em.componentCount<test>() == 0
                                               && em.componentCount<int>() == 1
                                               && em.componentCount<double>() == 0)
                                           && *em.getComponent<int>(id1) == 1, "Non-committed deletions should not change EntityManager contents");
                                    em.commitDeletions();
                                    expect(em.entityCount() == 1
                                           && (em.componentCount<test>() == 0
                                               && em.componentCount<int>() == 0
                                               && em.componentCount<double>() == 0), "Component deletion commit should delete it");

                                    em.insertComponent(2, id1);
                                    expect(*em.getComponent<int>(id1) == 2, "Inserted component should be of correct value");

                                    auto id2 = em.makeEntity();
                                    expect(em.entityCount() == 2
                                           && (em.componentCount<test>() == 0
                                               && em.componentCount<int>() == 1
                                               && em.componentCount<double>() == 0), "Entity and component counts should be correct after adding another entity");

                                    auto id3 = em.makeEntity();
                                    expect(em.entityCount() == 3
                                           && (em.componentCount<test>() == 0
                                               && em.componentCount<int>() == 1
                                               && em.componentCount<double>() == 0), "Entity and component counts should be correct after adding a third entity");

                                    em.insertComponent(3, id2);
                                    expect(em.componentCount<test>() == 0
                                           && em.componentCount<int>() == 2
                                           && em.componentCount<double>() == 0, "Component counts should be correct after adding a second component");
                                    expect(*em.getComponent<int>(id1) == 2
                                           && *em.getComponent<int>(id2) == 3, "Component values should be correct");

                                    auto allInts = em.getArchetype<int>();
                                    expect(allInts.size() == 2, "Archetype count should be correct");
                                    // Test values and indices
                                    expect((*allInts[0].components == 2 && allInts[0].entity == id1 && *allInts[1].components == 3 && allInts[1].entity == id2) ||
                                           (*allInts[1].components == 2 && allInts[1].entity == id1 && *allInts[0].components == 3 && allInts[0].entity == id2),
                                           "Archetype values should be correct");

                                    auto allDoubles = em.getArchetype<double>();
                                    expect(allDoubles.empty(), "Empty archetypes should be empty");

                                    auto allIntsAndDoubles = em.getArchetype<int, double>();
                                    expect(allIntsAndDoubles.empty(), "Empty archetypes should be empty");

                                    em.insertComponent(test{4}, id2);
                                    expect(em.componentCount<test>() == 1
                                           && em.componentCount<int>() == 2
                                           && em.componentCount<double>() == 0, "Component counts should be correct after adding a third component");
                                    expect(*em.getComponent<int>(id1) == 2
                                           && *em.getComponent<int>(id2) == 3
                                           && em.getComponent<test>(id2)->someValue == 4, "Component values should be correct");

                                    em.requestEntityDeletion(id1);
                                    expect(em.entityCount() == 3, "Entity count should not change before committing entity deletion");
                                    em.commitDeletions();
                                    expect(em.entityCount() == 2
                                           && (em.componentCount<test>() == 1
                                               && em.componentCount<int>() == 1
                                               && em.componentCount<double>() == 0), "Entity deletion commit should delete it with its components");
                                    expect(*em.getComponent<int>(id2) == 3, "The correct component should be deleted");
                                };
}
