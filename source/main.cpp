
#include "io/logger.h"
#include "application.h"
#include "core/scheduler.h"
#include "io/resource_pool.h"

#include <iostream>

int main() {
#ifndef NDEBUG
    Doughnut::Log::init(true, true, false, true);
#else
    Doughnut::Log::init(true, false, false, true);
#endif

#ifndef NDEBUG
    {
        Doughnut::ECS::testEntityManager();
        Doughnut::ECS::testSystemManager();
        Doughnut::testScheduler();
    }
#endif

    try {
//        Doughnut::ResourcePool<Doughnut::Graphics::Texture> texturePool{};
//        texturePool.preload("resources/textures/planet-albedo.png");
//        Doughnut::Log::i("Size of resource pool:", static_cast<double>(texturePool.size()) / (1024.0 * 1024.0), "MB");
//        return EXIT_SUCCESS;

        Doughnut::Application app{"Hello World!"};

        app.run();

        return EXIT_SUCCESS;

    } catch (const std::exception &e) {
        Doughnut::Log::flush();
        throw e;
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}