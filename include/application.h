//
// Created by Sam on 2023-04-08.
//

#ifndef DOUGHNUT_APPLICATION_H
#define DOUGHNUT_APPLICATION_H

#include "preprocessor.h"
#include "typedefs.h"
#include "io/window.h"
#include "io/input_manager.h"
#include "graphics/v1/renderer.h"

#include <memory>
#include <string>
#include <utility>

namespace Doughnut {
    class Application {
    public:
        explicit Application(std::string title) : mTitle(std::move(title)) {}

        void run();

        const std::string mTitle;
    private:
        void init();

        void mainLoop();

        void destroy();

        std::unique_ptr<EntitySystemManagerSpec> mESM;
        std::unique_ptr<Graphics::Renderer> mRenderer;
        std::unique_ptr<Window> mWindowManager;
        std::unique_ptr<InputController> mInputManager;

        Timer::Point mLastTimestamp = Timer::now();
        double mCurrentCpuWaitTime{};
        double mDeltaTime = 0;

        bool mMonkeyMode = true;
        bool mExitAfterMainLoop = true;
    };
};

#endif //DOUGHNUT_APPLICATION_H
