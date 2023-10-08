//
// Created by Sam on 2023-04-08.
//

#ifndef REALTIME_CELL_COLLAPSE_APPLICATION_H
#define REALTIME_CELL_COLLAPSE_APPLICATION_H

#include "preprocessor.h"
#include "typedefs.h"
#include "io/window_manager.h"
#include "io/input_manager.h"
#include "graphics/renderer.h"

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

        std::unique_ptr<EntitySystemManagerSpec> mESM;
        std::unique_ptr<GFX::Renderer> mRenderer;
        std::unique_ptr<WindowManager> mWindowManager;
        std::unique_ptr<InputController> mInputManager;

        Timer::Point mLastTimestamp = Timer::now();
        double mCurrentCpuWaitTime;
        double mDeltaTime = 0;

        bool mMonkeyMode = true;
        bool mExitAfterMainLoop = true;
    };
};

#endif //REALTIME_CELL_COLLAPSE_APPLICATION_H
