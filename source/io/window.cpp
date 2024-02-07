//
// Created by Sam on 2023-04-08.
//

#include "io/window.h"
#include "io/logger.h"

#include <iostream>

using namespace dn;

Window::Window(const std::string &t) {
    dn::log::i("Creating Window");

    this->title = t;

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    this->glfwWindow = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);
    this->monitor = glfwGetPrimaryMonitor();
    pollMonitorResolution();
}

void Window::updateTitle(const std::string &t) {
    this->title = t;
    if (this->glfwWindow != nullptr) {
        glfwSetWindowTitle(this->glfwWindow, this->title.c_str());
    }
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(this->glfwWindow);
}

void Window::close() const {
    glfwSetWindowShouldClose(this->glfwWindow, GLFW_TRUE);
}

void Window::toggleFullscreen() {
    this->isMaximized = !this->isMaximized;
    if (this->isMaximized) {
        pollPosition(); // For restoring later
        glfwSetWindowMonitor(this->glfwWindow, glfwGetPrimaryMonitor(),
                             0, 0,
                             this->monitorParams->width, this->monitorParams->height,
                             this->monitorParams->refreshRate);
    } else {
        glfwSetWindowMonitor(this->glfwWindow, nullptr,
                             this->windowPosX, this->windowPosY,
                             this->width, this->height, GLFW_DONT_CARE);
    }
}


void Window::pollMonitorResolution() {
    // Supposed to be valid until monitor disconnected, but seems to change between fullscreen and windowed
    this->monitorParams = glfwGetVideoMode(this->monitor);
    int count;
    auto params = glfwGetVideoModes(this->monitor, &count);

    for (int i = 0; i < count; ++i) {
        dn::log::d("Got monitor info:",
                         "\n\tWidth:\t", params[i].width,
                         "\n\tHeight:\t", params[i].height,
                         "\n\tRed Bits:\t", params[i].redBits,
                         "\n\tGreen Bits:\t", params[i].greenBits,
                         "\n\tBlue Bits:\t", params[i].blueBits,
                         "\n\tHz:\t", params[i].refreshRate);
        this->monitorParams = &params[i];
    }
}

void Window::pollPosition() {
    glfwGetWindowPos(this->glfwWindow, &this->windowPosX, &this->windowPosY);
}

Window::~Window() {
    dn::log::i("Destroying Window");

    glfwDestroyWindow(this->glfwWindow);

    glfwTerminate();
}