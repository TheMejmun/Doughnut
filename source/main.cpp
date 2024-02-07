
#include "io/logger.h"
#include "application.h"
#include "core/scheduler.h"
#include "io/resource_pool.h"
#include "graphics/renderer_v2.h"

#include <iostream>

int main() {
#ifndef NDEBUG
    dn::log::init(true, true, false, true);
#else
    dn::log::init(true, false, false, true);
#endif

#ifndef NDEBUG
    {
        dn::testEntityManager();
        dn::testSystemManager();
        dn::testScheduler();
    }
#endif

    try {
//        dn::ResourcePool<dn::Texture> texturePool{};
//        texturePool.preload("resources/textures/planet-albedo.png");
//        dn::log::i("Size of resource pool:", static_cast<double>(texturePool.size()) / (1024.0 * 1024.0), "MB");
//        return EXIT_SUCCESS;

        dn::Window window{"TEST"};
        dn::RendererV2 renderer{&window, "TEST"};
        return EXIT_SUCCESS;

        dn::Application app{"Hello World!"};

        app.run();

        return EXIT_SUCCESS;

    } catch (const std::exception &e) {
        dn::log::flush();
        throw e;
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}