//
// Created by Sam on 2023-04-08.
//

#ifndef DOUGHNUT_WINDOW_MANAGER_H
#define DOUGHNUT_WINDOW_MANAGER_H

#include "preprocessor.h"

#include <GLFW/glfw3.h>
#include <memory>
#include <string>

const int32_t DEFAULT_WIDTH = 1920;
const int32_t DEFAULT_HEIGHT = 1080;

class WindowManager {
public:
    WindowManager(const std::string &title);

    ~WindowManager();

    void updateTitle(const std::string &title);

    [[nodiscard]] bool shouldClose() const;

    void close() const;

    void toggleFullscreen();

    GLFWwindow *window = nullptr;

    bool isMaximized = false;
private:
    void pollMonitorResolution();

    void pollWindowPosition();

    int32_t width = DEFAULT_WIDTH, height = DEFAULT_HEIGHT;
    int windowPosX = 0, windowPosY = 0;

    GLFWmonitor *monitor = nullptr;
    const GLFWvidmode *monitorParams = nullptr;
    std::string title;
};

#endif //DOUGHNUT_WINDOW_MANAGER_H
