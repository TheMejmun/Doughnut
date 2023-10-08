//
// Created by Sam on 2023-10-08.
//

#include "ecs/entities/dense_sphere.h"
#include "util/importer.h"

// TODO: Mesh as static resource, not hot-loading
void DenseSphere::upload(EntityManagerSpec &em) {
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
