//
// Created by Sam on 2023-04-08.
//

#ifndef DOUGHNUT_WINDOW_H
#define DOUGHNUT_WINDOW_H

#include "preprocessor.h"

#include <GLFW/glfw3.h>
#include <memory>
#include <string>

namespace dn {
    class Window {
    public:
        explicit Window(std::string title, int32_t width = 640, int32_t height = 400, bool resizable = true);

        ~Window();

        void updateTitle(const std::string &title);

        [[nodiscard]] bool shouldClose() const;

        void close() const;

        void toggleFullscreen();

        GLFWwindow *mGlfwWindow = nullptr;
        std::string mTitle;

    private:
        void pollMonitorResolution();

        void pollPosition();

        bool mIsMaximized = false;
        int32_t mWidth, mHeight;
        int mWindowPosX = 0, mWindowPosY = 0;

        GLFWmonitor *mMonitor = nullptr;
        const GLFWvidmode *mMonitorParams = nullptr;
    };
}

#endif //DOUGHNUT_WINDOW_H
