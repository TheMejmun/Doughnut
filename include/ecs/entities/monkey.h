//
// Created by Saman on 31.08.23.
//

#ifndef REALTIME_CELL_COLLAPSE_MONKEY_H
#define REALTIME_CELL_COLLAPSE_MONKEY_H

#include "preprocessor.h"
#include "util/importer.h"

// TODO: Mesh as static resource, not hot-loading
namespace Monkey {
    template<class ENTITY_MANAGER>
    void upload(ENTITY_MANAGER &em) {
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
}

#endif //REALTIME_CELL_COLLAPSE_MONKEY_H
