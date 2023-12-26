
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

    try {
        Doughnut::ResourcePool<Doughnut::Graphics::Texture> texturePool{};
        texturePool.preload("resources/textures/planet-albedo.png");
        return EXIT_SUCCESS;

//    Doughnut::ECS::benchmarkEntityManager(10'000'000);
//    return 0;

//    Doughnut::Log::benchmarkLogger(1'000'000);
//    return 0;

#ifndef NDEBUG
        {
            Doughnut::ECS::testEntityManager();
            Doughnut::ECS::testSystemManager();
            Doughnut::testScheduler();
        }
#endif

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