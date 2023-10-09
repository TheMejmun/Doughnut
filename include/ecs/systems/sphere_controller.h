//
// Created by Saman on 01.08.23.
//

#ifndef REALTIME_CELL_COLLAPSE_SPHERE_CONTROLLER_H
#define REALTIME_CELL_COLLAPSE_SPHERE_CONTROLLER_H

#include "preprocessor.h"
#include "typedefs.h"

class SphereController : public ECS2::System<EntityManagerSpec> {
public:
    void update(double delta, EntityManagerSpec &entityManager) override;
private:
    bool mDoSphereRotation = false;
};

#endif //REALTIME_CELL_COLLAPSE_SPHERE_CONTROLLER_H
