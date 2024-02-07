//
// Created by Sam on 2023-10-08.
//

#ifndef DOUGHNUT_TYPEDEFS_H
#define DOUGHNUT_TYPEDEFS_H

#include "ecs/entity_system_manager.h"
#include "ecs/components/input_state.h"
#include "ecs/components/render_mesh.h"
#include "ecs/components/render_mesh_simplifiable.h"
#include "ecs/components/projector.h"
#include "ecs/components/rotating_sphere.h"
#include "physics/transformer.h"
#include "ecs/components/ui_state.h"

typedef dn::EntityManager<
        InputState,
        UiState,
        RenderMesh,
        RenderMeshSimplifiable,
        Transformer4,
        Projector,
        RotatingSphere
> EntityManagerSpec;

typedef dn::EntitySystemManager<
        1,
        EntityManagerSpec
> EntitySystemManagerSpec;

#endif //DOUGHNUT_TYPEDEFS_H
