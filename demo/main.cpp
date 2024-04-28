
#include "io/window.h"
#include "graphics/render_api.h"
#include "graphics/renderable.h"
#include "util/timer.h"

class Renderer {
public:
    explicit Renderer(dn::Window &window) : mAPI(window) {}

    void drawFrame(double delta) {
        mAPI.nextImage();
        mAPI.startRecording();
        mAPI.beginRenderPass();
        mAPI.recordDraw(mRenderable);
        mAPI.recordUiDraw();
        mAPI.endRenderPass();
        mAPI.endRecording();
        mAPI.drawFrame(delta);
    }

private:
    dn::VulkanAPI mAPI;

    dn::Renderable mRenderable{
            dn::Texture("../resources/textures/debug.png"),
            "../resources/shaders/quad.vert",
            "../resources/shaders/quad.frag",
            "../resources/models/quad.glb",
            {}
    };
};

int main() {
#ifndef NDEBUG
    dn::log::init(true, true, true, true);
#else
    dn::log::init(true, false, false, true);
#endif

    dn::Window window{"Hello World", 800, 600, true};
    Renderer renderer{window};

    dn::FPSCounter fps{};

    dn::Time lastTimestamp = dn::now();
    double delta;
    while (!window.shouldClose()) {
        dn::Time time = dn::now();
        delta = dn::duration(lastTimestamp, time);
        lastTimestamp = time;

        fps.update(delta);

        window.poll();

        renderer.drawFrame(delta);
    }

    return 0;
}
