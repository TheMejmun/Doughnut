//
// Created by Sam on 2023-04-08.
//

#ifndef REALTIME_CELL_COLLAPSE_APPLICATION_H
#define REALTIME_CELL_COLLAPSE_APPLICATION_H

#include "preprocessor.h"
#include "graphics/renderer.h"
#include "util/timer.h"
#include "io/window_manager.h"
#include "io/input_manager.h"
#include "ecs/ecs.h"
#include "io/printer.h"

#include <memory>
#include <string>

namespace Doughnut {
    class Application {
    public:
        void run();

        std::string title;
    private:
        void init();

        void mainLoop();

        void destroy();

        ECS ecs{};

        std::unique_ptr<GFX::Renderer> renderer;
        WindowManager windowManager{};
        InputController inputManager{};

        Timer::Point lastTimestamp = Timer::now();
        double currentCpuWaitTime;
        uint32_t currentFPS = 0;
        double deltaTime = 0;

        bool monkeyMode = true;
        bool exitAfterMainLoop = true;
    };
};

#endif //REALTIME_CELL_COLLAPSE_APPLICATION_H
