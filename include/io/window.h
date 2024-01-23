//
// Created by Sam on 2023-04-08.
//

#ifndef DOUGHNUT_WINDOW_H
#define DOUGHNUT_WINDOW_H

#include "preprocessor.h"

#include <GLFW/glfw3.h>
#include <memory>
#include <string>

const int32_t DEFAULT_WIDTH = 1920;
const int32_t DEFAULT_HEIGHT = 1080;

namespace Doughnut {
    class Window {
    public:
        explicit Window(const std::string &title);

        ~Window();

        void updateTitle(const std::string &title);

        [[nodiscard]] bool shouldClose() const;

        void close() const;

        void toggleFullscreen();

        GLFWwindow *glfwWindow = nullptr;

        bool isMaximized = false;
    private:
        void pollMonitorResolution();

        void pollPosition();

        int32_t width = DEFAULT_WIDTH, height = DEFAULT_HEIGHT;
        int windowPosX = 0, windowPosY = 0;

        GLFWmonitor *monitor = nullptr;
        const GLFWvidmode *monitorParams = nullptr;
        std::string title;
    };
}

#endif //DOUGHNUT_WINDOW_H
