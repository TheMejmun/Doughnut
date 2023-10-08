//
// Created by Saman on 31.07.23.
//

#ifndef REALTIME_CELL_COLLAPSE_CAMERA_H
#define REALTIME_CELL_COLLAPSE_CAMERA_H

#include "preprocessor.h"
#include "physics/transformer.h"
#include "ecs/components/projector.h"

namespace Camera {
    template<class ENTITY_MANAGER>
    void upload(ENTITY_MANAGER &em) {
        auto id = em.makeEntity();

        Transformer4 transform{};
        transform.translate(glm::vec3(0, 0, -2.65));
        em.insertComponent(transform, id);

        em.template insertComponent<Projector>(id);
    }
}

#endif //REALTIME_CELL_COLLAPSE_CAMERA_H
