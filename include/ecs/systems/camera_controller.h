//
// Created by Saman on 31.07.23.
//

#ifndef REALTIME_CELL_COLLAPSE_CAMERA_CONTROLLER_H
#define REALTIME_CELL_COLLAPSE_CAMERA_CONTROLLER_H

#include "preprocessor.h"
#include "typedefs.h"

class CameraController : public ECS2::System<EntityManagerSpec> {
public:
    void update(double delta, EntityManagerSpec &entityManager) override;
};

#endif //REALTIME_CELL_COLLAPSE_CAMERA_CONTROLLER_H
