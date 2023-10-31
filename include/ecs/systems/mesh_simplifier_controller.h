//
// Created by Saman on 27.08.23.
//

#ifndef REALTIME_CELL_COLLAPSE_MESH_SIMPLIFIER_CONTROLLER_H
#define REALTIME_CELL_COLLAPSE_MESH_SIMPLIFIER_CONTROLLER_H

#include "preprocessor.h"
#include "typedefs.h"

class MeshSimplifierController : public Doughnut::ECS::System<EntityManagerSpec> {
public:
    ~MeshSimplifierController() noexcept override;

    void update(double delta, EntityManagerSpec &entityManager) override;
};

#endif //REALTIME_CELL_COLLAPSE_MESH_SIMPLIFIER_CONTROLLER_H
