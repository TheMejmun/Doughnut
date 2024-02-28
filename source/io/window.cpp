//
// Created by Sam on 2023-04-08.
//

#include "io/window.h"
#include "io/logger.h"
#include "util/require.h"

#include <iostream>
#include <utility>

using namespace dn;

Window::Window(std::string title, int32_t width, int32_t height, bool resizable)
        : mTitle(std::move(title)),
          mWidth(width),
          mHeight(height) {
    require(mWidth > 0 && mHeight > 0, "Can not create a window with negative dimensions");
    dn::log::d("Creating Window");

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    mGlfwWindow = glfwCreateWindow(mWidth, mHeight, mTitle.c_str(), nullptr, nullptr);
    glfwSetWindowAttrib(mGlfwWindow, GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
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

Size Window::getSize() const {
    int fbWidth, fbHeight, wWidth, wHeight;
    glfwGetFramebufferSize(mGlfwWindow, &fbWidth, &fbHeight);
    glfwGetWindowSize(mGlfwWindow, &wWidth, &wHeight);

    float scale = (static_cast<float>(fbWidth) / static_cast<float>(wWidth));
    return {
            static_cast<uint32_t>(wWidth),
            static_cast<uint32_t>(wHeight),
            static_cast<uint32_t>(fbWidth),
            static_cast<uint32_t>(fbHeight),
            scale
    };
}

Window::~Window() {
    dn::log::d("Destroying Window");

    glfwDestroyWindow(mGlfwWindow);

    glfwTerminate();
}