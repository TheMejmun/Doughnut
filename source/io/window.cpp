//
// Created by Sam on 2023-04-08.
//

#include "io/window.h"
#include "io/logger.h"
#include "util/require.h"
#include "util/os.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <utility>

#ifdef OS_WINDOWS
#define SDL_VIDEODRIVER windows
#endif
// TODO other OSes

using namespace dn;

// TODO Poll Events https://vkguide.dev/docs/chapter-0/code_walkthrough/

PixelFormat mapFormat(SDL_PixelFormatEnum sdlFormat) {
    switch (sdlFormat) {
        case SDL_PIXELFORMAT_INDEX1LSB:
            return INDEX1LSB;
        case SDL_PIXELFORMAT_INDEX1MSB:
            return INDEX1MSB;
        case SDL_PIXELFORMAT_INDEX4LSB:
            return INDEX4LSB;
        case SDL_PIXELFORMAT_INDEX4MSB:
            return INDEX4MSB;
        case SDL_PIXELFORMAT_INDEX8:
            return INDEX8;
        case SDL_PIXELFORMAT_RGB332:
            return RGB332;
        case SDL_PIXELFORMAT_XRGB4444:
            return XRGB4444;
        case SDL_PIXELFORMAT_XBGR4444:
            return XBGR4444;
        case SDL_PIXELFORMAT_XRGB1555:
            return XRGB1555;
        case SDL_PIXELFORMAT_XBGR1555:
            return XBGR1555;
        case SDL_PIXELFORMAT_ARGB4444:
            return ARGB4444;
        case SDL_PIXELFORMAT_RGBA4444:
            return RGBA4444;
        case SDL_PIXELFORMAT_ABGR4444:
            return ABGR4444;
        case SDL_PIXELFORMAT_BGRA4444:
            return BGRA4444;
        case SDL_PIXELFORMAT_ARGB1555:
            return ARGB1555;
        case SDL_PIXELFORMAT_RGBA5551:
            return RGBA5551;
        case SDL_PIXELFORMAT_ABGR1555:
            return ABGR1555;
        case SDL_PIXELFORMAT_BGRA5551:
            return BGRA5551;
        case SDL_PIXELFORMAT_RGB565:
            return RGB565;
        case SDL_PIXELFORMAT_BGR565:
            return BGR565;
        case SDL_PIXELFORMAT_RGB24:
            return RGB24;
        case SDL_PIXELFORMAT_BGR24:
            return BGR24;
        case SDL_PIXELFORMAT_XRGB8888:
            return XRGB8888;
        case SDL_PIXELFORMAT_RGBX8888:
            return RGBX8888;
        case SDL_PIXELFORMAT_XBGR8888:
            return XBGR8888;
        case SDL_PIXELFORMAT_BGRX8888:
            return BGRX8888;
        case SDL_PIXELFORMAT_ARGB8888:
            return ARGB8888;
        case SDL_PIXELFORMAT_RGBA8888:
            return RGBA8888;
        case SDL_PIXELFORMAT_ABGR8888:
            return ABGR8888;
        case SDL_PIXELFORMAT_BGRA8888:
            return BGRA8888;
        case SDL_PIXELFORMAT_ARGB2101010:
            return ARGB2101010;
        case SDL_PIXELFORMAT_YV12:
            return YV12;
        case SDL_PIXELFORMAT_IYUV:
            return IYUV;
        case SDL_PIXELFORMAT_YUY2:
            return YUY2;
        case SDL_PIXELFORMAT_UYVY:
            return UYVY;
        case SDL_PIXELFORMAT_YVYU:
            return YVYU;
        case SDL_PIXELFORMAT_NV12:
            return NV12;
        case SDL_PIXELFORMAT_NV21:
            return NV21;
        default:
        error("Unknown pixel format");
    }
}

std::string toString(PixelFormat pixelFormat) {
    switch (pixelFormat) {
        case INDEX1LSB:
            return "INDEX1LSB";
        case INDEX1MSB:
            return "INDEX1MSB";
        case INDEX4LSB:
            return "INDEX4LSB";
        case INDEX4MSB:
            return "INDEX4MSB";
        case INDEX8:
            return "INDEX8";
        case RGB332:
            return "RGB332";
        case XRGB4444:
            return "XRGB4444";
        case XBGR4444:
            return "XBGR4444";
        case XRGB1555:
            return "XRGB1555";
        case XBGR1555:
            return "XBGR1555";
        case ARGB4444:
            return "ARGB4444";
        case RGBA4444:
            return "RGBA4444";
        case ABGR4444:
            return "ABGR4444";
        case BGRA4444:
            return "BGRA4444";
        case ARGB1555:
            return "ARGB1555";
        case RGBA5551:
            return "RGBA5551";
        case ABGR1555:
            return "ABGR1555";
        case BGRA5551:
            return "BGRA5551";
        case RGB565:
            return "RGB565";
        case BGR565:
            return "BGR565";
        case RGB24:
            return "RGB24";
        case BGR24:
            return "BGR24";
        case XRGB8888:
            return "XRGB8888";
        case RGBX8888:
            return "RGBX8888";
        case XBGR8888:
            return "XBGR8888";
        case BGRX8888:
            return "BGRX8888";
        case ARGB8888:
            return "ARGB8888";
        case RGBA8888:
            return "RGBA8888";
        case ABGR8888:
            return "ABGR8888";
        case BGRA8888:
            return "BGRA8888";
        case ARGB2101010:
            return "ARGB2101010";
        case YV12:
            return "YV12";
        case IYUV:
            return "IYUV";
        case YUY2:
            return "YUY2";
        case UYVY:
            return "UYVY";
        case YVYU:
            return "YVYU";
        case NV12:
            return "NV12";
        case NV21:
            return "NV21";
        default:
        error("Unknown pixel format");
    }
}

Window::Window(std::string title, int32_t width, int32_t height, bool resizable) : mTitle(std::move(title)) {
    require(width > 0 && height > 0, "Can not set negative window dimensions");

    // Specify SDL subsystems to init
    // SDL_INIT_GAMECONTROLLER for imgui?
    bool success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    log::i("Test 1");
    require(success == 0, SDL_GetError());

    log::i("Test -1");
    require(SDL_VideoInit(nullptr) == 0, SDL_GetError());

    pollMonitorResolution();

    // TODO see what this does
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    uint32_t windowFlags = SDL_WINDOW_VULKAN;
    if (resizable)
        windowFlags |= SDL_WINDOW_RESIZABLE;
    // TODO maybe for mac
//    windowFlags |= SDL_WINDOW_ALLOW_HIGHDPI;

    mHandle = SDL_CreateWindow(mTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);
    log::i("Test 2");
    require(mHandle != nullptr, SDL_GetError());

    mWindowID = SDL_GetWindowID((SDL_Window *) mHandle);

    pollPosition();

    dn::log::i("Video driver:", SDL_GetCurrentVideoDriver());

    dn::log::d("Created Window");
}

void Window::updateTitle(const std::string &t) {
    mTitle = t;
    SDL_SetWindowTitle((SDL_Window *) mHandle, mTitle.c_str());
}

bool Window::shouldClose() const {
    return mShouldClose;
}

void Window::close() const {
    SDL_Event event{};
    event.window = {SDL_WINDOWEVENT,
                    SDL_GetTicks(),
                    mWindowID,
                    SDL_WINDOWEVENT_CLOSE
    };
    SDL_PushEvent(&event);
}

void Window::toggleFullscreen() {
    mIsMaximized = !mIsMaximized;
    if (mIsMaximized) {
        pollPosition(); // For restoring later
        SDL_SetWindowFullscreen((SDL_Window *) mHandle, SDL_WINDOW_FULLSCREEN);
    } else {
        // TODO set position?
        SDL_SetWindowFullscreen((SDL_Window *) mHandle, 0);
    }
}

void Window::listen(WindowEventListener *listener) {
    mEventListeners.emplace_back(listener);
}

void Window::poll() {
    // Should only be called on main window thread
    // https://wiki.libsdl.org/SDL2/SDL_PumpEvents
    // Does not seem to make a difference anyway
    SDL_PumpEvents();

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        for (auto &listener: mEventListeners) {
            listener->onWindowEvent(event);
        }

        switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    mShouldClose = true;
                }
                break;
        }
    }
}

void Window::pollMonitorResolution() {
    // Supposed to be valid until monitor disconnected, but seems to change
    // between fullscreen and windowed
    int numDisplays = SDL_GetNumVideoDisplays();
    require(numDisplays > 0, "No displays were found");
    mDisplayInfos.resize(numDisplays);

    for (int displayIndex = 0; displayIndex < numDisplays; ++displayIndex) {
        int numDisplayModes = SDL_GetNumDisplayModes(displayIndex);
        DisplayInfo &displayInfo = mDisplayInfos[displayIndex];

        displayInfo.name = SDL_GetDisplayName(displayIndex);

        displayInfo.displayModes.resize(numDisplayModes);

        for (int displayModeIndex = 0; displayModeIndex < numDisplayModes; ++displayModeIndex) {
            DisplayMode &displayMode = displayInfo.displayModes[displayModeIndex];

            SDL_DisplayMode sdlDisplayMode;
            auto result = SDL_GetDisplayMode(displayIndex, displayModeIndex, &sdlDisplayMode);
            require(result == 0, SDL_GetError());

            displayMode.width = sdlDisplayMode.w;
            displayMode.height = sdlDisplayMode.h;
            displayMode.refreshRate = sdlDisplayMode.refresh_rate;

            auto pixelOrder = SDL_PIXELORDER(sdlDisplayMode.format); // https://wiki.libsdl.org/SDL2/SDL_PixelFormatEnum#order
            auto pixelBitPattern = SDL_PIXELLAYOUT(sdlDisplayMode.format); // https://wiki.libsdl.org/SDL2/SDL_PixelFormatEnum#layout

            displayMode.format = mapFormat((SDL_PixelFormatEnum) sdlDisplayMode.format);
            displayMode.hasAlpha = SDL_ISPIXELFORMAT_ALPHA(sdlDisplayMode.format);

            dn::log::d("Got monitor info for", displayInfo.name, "\b:",
                       "\n\tWidth:\t", displayMode.width,
                       "\n\tHeight:\t", displayMode.height,
                       "\n\tFormat:\t", toString(displayMode.format),
                       "\n\tHz:\t", sdlDisplayMode.refresh_rate
            );
        }
    }
}

void Window::pollPosition() {
    SDL_GetWindowPosition((SDL_Window *) mHandle, &mWindowPosX, &mWindowPosY);
}

Size Window::getSize() const {
    int fbWidth, fbHeight, wWidth, wHeight;
    SDL_GetWindowSize((SDL_Window *) mHandle, &wWidth, &wHeight);
    // TODO use correct dimensions for scale
    SDL_GetWindowSize((SDL_Window *) mHandle, &fbWidth, &fbHeight);
    // SDL_GetRendererOutputSize((SDL_Renderer *) mRendererHandle, &fbWidth, &fbHeight);

    // Assume uniform scaling
    float scale = (static_cast<float>(fbWidth) / static_cast<float>(wWidth));
    return {static_cast<uint32_t>(wWidth),
            static_cast<uint32_t>(wHeight),
            static_cast<uint32_t>(fbWidth),
            static_cast<uint32_t>(fbHeight),
            scale
    };
}

Window::~Window() {
    dn::log::d("Destroying Window");

    SDL_DestroyWindow((SDL_Window *) mHandle);
    SDL_Quit();
}