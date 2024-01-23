//
// Created by Saman on 27.08.23.
//

#ifndef DOUGHNUT_MESH_SIMPLIFIER_CONTROLLER_H
#define DOUGHNUT_MESH_SIMPLIFIER_CONTROLLER_H

#include "preprocessor.h"
#include "typedefs.h"

class MeshSimplifierController : public Doughnut::ECS::System<EntityManagerSpec> {
public:
    ~MeshSimplifierController() noexcept override;

    void update(double delta, EntityManagerSpec &entityManager) override;
};

#endif //DOUGHNUT_MESH_SIMPLIFIER_CONTROLLER_H
