//
// Created by Saman on 01.08.23.
//

#ifndef DOUGHNUT_SPHERE_CONTROLLER_H
#define DOUGHNUT_SPHERE_CONTROLLER_H

#include "preprocessor.h"
#include "typedefs.h"

class SphereController : public dn::System<EntityManagerSpec> {
public:
    void update(double delta, EntityManagerSpec &entityManager) override;
private:
    bool mDoSphereRotation = false;
};

#endif //DOUGHNUT_SPHERE_CONTROLLER_H
