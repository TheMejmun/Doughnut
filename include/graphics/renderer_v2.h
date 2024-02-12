//
// Created by Sam on 2024-01-23.
//

#ifndef DOUGHNUT_RENDERER_V2_H
#define DOUGHNUT_RENDERER_V2_H

#include "preprocessor.h"
#include "render_api.h"
#include "io/window.h"
#include "io/resource_pool.h"
#include "shader.h"

#include <GLFW/glfw3.h>
#include <string>

#define WIREFRAME_MODE

namespace dn {
    class RendererV2 {
    public:
        explicit RendererV2(Window& window);

        ~RendererV2();

        void drawFrame(double delta);

    private:
        VulkanAPI mAPI;

        ResourcePool<Texture> texturePool{};
        ResourcePool<Shader> shaderPool{};
    };
}

#endif //DOUGHNUT_RENDERER_V2_H
