//
// Created by Saman on 18.04.23.
//

#ifndef REALTIME_CELL_COLLAPSE_DENSE_SPHERE_H
#define REALTIME_CELL_COLLAPSE_DENSE_SPHERE_H

#include "preprocessor.h"
#include "util/importer.h"

// TODO: Mesh as static resource, not hot-loading
namespace DenseSphere {
    template<class ENTITY_MANAGER>
    void upload(ENTITY_MANAGER &em) {
        auto id = em.makeEntity();

        auto mesh = Importinator::importMesh("resources/models/dense_sphere.glb");

        RenderMesh renderMesh{};
        renderMesh.indices = std::move(mesh.indices);
        renderMesh.vertices = std::move(mesh.vertices);
        for (auto &v: renderMesh.vertices) {
            v.color = Color::random().getLAB();
        }
        em.insertComponent(renderMesh, id);

        // Inplace creation, because mutexes can't be copied
        auto renderMeshSimplifiable = em.template insertComponent<RenderMeshSimplifiable>(id);
        renderMeshSimplifiable->simplifiedMeshMutex = std::make_unique<std::mutex>();

        Transformer4 transform{};
        em.insertComponent(transform, id);

        RotatingSphere rotatingSphereComponent{};
        em.insertComponent(rotatingSphereComponent, id);
    }
}

#endif //REALTIME_CELL_COLLAPSE_DENSE_SPHERE_H
