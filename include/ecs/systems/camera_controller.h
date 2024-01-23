//
// Created by Saman on 31.07.23.
//

#ifndef DOUGHNUT_CAMERA_CONTROLLER_H
#define DOUGHNUT_CAMERA_CONTROLLER_H

#include "preprocessor.h"
#include "typedefs.h"

class CameraController : public Doughnut::ECS::System<EntityManagerSpec> {
public:
    void update(double delta, EntityManagerSpec &entityManager) override;
};

#endif //DOUGHNUT_CAMERA_CONTROLLER_H
