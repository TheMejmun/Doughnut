//
// Created by Sam on 2023-04-08.
//

#ifndef DOUGHNUT_WINDOW_H
#define DOUGHNUT_WINDOW_H

#include "preprocessor.h"

#include <memory>
#include <string>
#include <vector>

namespace dn {
    enum PixelFormat {
        UNKNOWN,
        INDEX1LSB,
        INDEX1MSB,
        INDEX4LSB,
        INDEX4MSB,
        INDEX8,
        RGB332,
        XRGB4444,
        XBGR4444,
        XRGB1555,
        XBGR1555,
        ARGB4444,
        RGBA4444,
        ABGR4444,
        BGRA4444,
        ARGB1555,
        RGBA5551,
        ABGR1555,
        BGRA5551,
        RGB565,
        BGR565,
        RGB24,
        BGR24,
        XRGB8888,
        RGBX8888,
        XBGR8888,
        BGRX8888,
        ARGB8888,
        RGBA8888,
        ABGR8888,
        BGRA8888,
        ARGB2101010,
        YV12,
        IYUV,
        YUY2,
        UYVY,
        YVYU,
        NV12,
        NV21
    };

    struct Size {
        uint32_t windowWidth;
        uint32_t windowHeight;
        uint32_t framebufferWidth;
        uint32_t framebufferHeight;
        float scale;
    };

    struct DisplayMode {
        uint32_t width;
        uint32_t height;
        uint32_t refreshRate;
        PixelFormat format;
        bool hasAlpha;
    };

    struct DisplayInfo {
        std::vector<DisplayMode> displayModes{};
        std::string name;
    };

    class Window {
    public:
        explicit Window(std::string title, int32_t width = 640, int32_t height = 400, bool resizable = true);

        ~Window();

        void updateTitle(const std::string &title);

        [[nodiscard]] bool shouldClose() const;

        void close() const;

        void toggleFullscreen();

        void poll();

        [[nodiscard]] Size getSize() const;

        void *mHandle = nullptr; // SDL_Window
        std::string mTitle;

    private:
        void pollMonitorResolution();

        void pollPosition();

        bool mIsMaximized = false;
        int mWindowPosX = 0, mWindowPosY = 0;

        uint32_t mWindowID;
        bool mShouldClose = false; // TODO Write when event received

        std::vector<DisplayInfo> mDisplayInfos{};
    };
}

#endif //DOUGHNUT_WINDOW_H
