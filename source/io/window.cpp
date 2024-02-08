//
// Created by Sam on 2023-04-08.
//

#include "io/window.h"
#include "io/logger.h"

#include <iostream>

using namespace dn;

Window::Window(const std::string &t) {
    dn::log::d("Creating Window");

    mTitle = t;

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    mGlfwWindow = glfwCreateWindow(mWidth, mHeight, mTitle.c_str(), nullptr, nullptr);
    mMonitor = glfwGetPrimaryMonitor();
    pollMonitorResolution();
}

void Window::updateTitle(const std::string &t) {
    mTitle = t;
    if (mGlfwWindow != nullptr) {
        glfwSetWindowTitle(mGlfwWindow, mTitle.c_str());
    }
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(mGlfwWindow);
}

void Window::close() const {
    glfwSetWindowShouldClose(mGlfwWindow, GLFW_TRUE);
}

void Window::toggleFullscreen() {
    mIsMaximized = !mIsMaximized;
    if (mIsMaximized) {
        pollPosition(); // For restoring later
        glfwSetWindowMonitor(mGlfwWindow, glfwGetPrimaryMonitor(),
                             0, 0,
                             mWidth, mHeight,
                             mMonitorParams->refreshRate);
    } else {
        glfwSetWindowMonitor(mGlfwWindow, nullptr,
                             mWindowPosX, mWindowPosY,
                             mWidth, mHeight, GLFW_DONT_CARE);
    }
}


void Window::pollMonitorResolution() {
    // Supposed to be valid until monitor disconnected, but seems to change between fullscreen and windowed
    mMonitorParams = glfwGetVideoMode(mMonitor);
    int count;
    auto params = glfwGetVideoModes(mMonitor, &count);

    for (int i = 0; i < count; ++i) {
        dn::log::d("Got monitor info:",
                   "\n\tWidth:\t", params[i].width,
                   "\n\tHeight:\t", params[i].height,
                   "\n\tRed Bits:\t", params[i].redBits,
                   "\n\tGreen Bits:\t", params[i].greenBits,
                   "\n\tBlue Bits:\t", params[i].blueBits,
                   "\n\tHz:\t", params[i].refreshRate);
        mMonitorParams = &params[i];
    }
}

void Window::pollPosition() {
    glfwGetWindowPos(mGlfwWindow, &mWindowPosX, &mWindowPosY);
}

std::string Window::getTitle() {
    return mTitle;
}

Window::~Window() {
    dn::log::d("Destroying Window");

    glfwDestroyWindow(mGlfwWindow);

    glfwTerminate();
}