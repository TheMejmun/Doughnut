//
// Created by Sam on 2023-10-08.
//

#include "ecs/entities/monkey.h"
#include "util/importer.h"

void Monkey::upload(EntityManagerSpec &em) {
    auto id = em.makeEntity();

    auto mesh = Importinator::importMesh("resources/models/monkey.glb");

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
    transform.rotate(glm::radians(180.0), glm::vec3(0, 1, 0));
    em.insertComponent(transform, id);

    RotatingSphere rotatingSphereComponent{};
    em.insertComponent(rotatingSphereComponent, id);
}