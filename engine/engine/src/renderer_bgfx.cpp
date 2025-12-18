#include "nyanchu/renderer.h"
#include "platform/platform_utils.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>

#include <iostream>

#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#endif

namespace nyanchu {

class BGFXRenderer : public IRenderer
{
public:
    BGFXRenderer() = default;
    ~BGFXRenderer() override = default;

    bool initialize(GLFWwindow* window, uint32_t width, uint32_t height) override
    {
        bgfx::PlatformData pd;

#if BX_PLATFORM_OSX
        pd.nwh = glfwGetCocoaWindow(window);
        pd.ndt = nullptr;
#elif BX_PLATFORM_LINUX
        pd.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
        pd.ndt = glfwGetX11Display();
#elif BX_PLATFORM_WINDOWS
        pd.nwh = glfwGetWin32Window(window);
        pd.ndt = nullptr;
#endif

        bgfx::Init bgfxInit;
        bgfxInit.type = bgfx::RendererType::Count; // Auto-select renderer
        bgfxInit.resolution.width = width;
        bgfxInit.resolution.height = height;
        bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
        bgfxInit.platformData = pd;

        if (!bgfx::init(bgfxInit))
        {
            std::cerr << "Failed to initialize BGFX" << std::endl;
            return false;
        }

        // Set view 0 clear state
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, (uint16_t)width, (uint16_t)height);

        std::cout << "BGFXRenderer initialized successfully." << std::endl;

        return true;
    }

    void shutdown() override
    {
        bgfx::shutdown();
        std::cout << "BGFXRenderer shutdown." << std::endl;
    }

    void beginFrame() override
    {
        bgfx::touch(0);
    }

    void endFrame() override
    {
        bgfx::frame();
    }

    void resize(uint32_t width, uint32_t height) override
    {
        bgfx::reset(width, height, BGFX_RESET_VSYNC);
        bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);
    }

    void drawMesh(const Mesh& mesh, const glm::mat4& modelMatrix) override { /* Empty */ }
    void drawTriangle() override { /* Empty */ }
    void drawCube(const glm::mat4& modelMatrix) override { /* Empty */ }
};

std::unique_ptr<IRenderer> createBGFXRenderer() {
    return std::make_unique<BGFXRenderer>();
}

} // namespace nyanchu
