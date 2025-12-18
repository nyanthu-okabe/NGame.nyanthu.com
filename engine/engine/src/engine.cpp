#include "nyanchu/engine.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "platform/platform_utils.h"
#include "nyanchu/renderer.h" // Include for IRenderer and createBGFXRenderer()

namespace nyanchu {

// GLFW framebuffer resize callback
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    auto engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (engine) {
        engine->resize(width, height);
    }
}

Engine::Engine() : m_window(nullptr) {} // m_window will be initialized in init()

Engine::~Engine() {
    // shutdown should be called explicitly
}

void Engine::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    // For now, let's create a window here. This might be configurable later.
    m_window = glfwCreateWindow(800, 600, "Nyanthu Engine", NULL, NULL);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_window);

    // Set up resize callback
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    // --- 削除 ---
    // 理由: プラットフォーム固有のレンダラー選択を削除。
    // createBGFXRenderer() が BGFXRenderer をインスタンス化し、
    // BGFXRenderer 内で bgfx::Init(bgfx::RendererType::Count) により
    // 実行時に最適なバックエンドが自動選択されるため、ここでは分岐不要。
    m_renderer = createBGFXRenderer();

    if (!m_renderer->initialize(m_window, 800, 600)) {
        std::cerr << "Failed to initialize Renderer" << std::endl;
        return;
    }

    m_audio = std::make_unique<Audio>();
    m_audio->init();

    m_resourceDir = getExecutableDir();

    m_isRunning = true;
    std::cout << "Engine initialized" << std::endl;
}

void Engine::shutdown() {
    m_audio->shutdown();
    m_renderer->shutdown();
    glfwDestroyWindow(m_window);
    glfwTerminate();
    std::cout << "Engine shutdown" << std::endl;
}

bool Engine::isRunning() {
    return m_isRunning && !glfwWindowShouldClose(m_window);
}

void Engine::pollEvents() {
    glfwPollEvents();
}

void Engine::beginFrame() {
    m_renderer->beginFrame();
}

void Engine::endFrame() {
    m_renderer->endFrame();
    glfwSwapBuffers(m_window);
}

void Engine::resize(int width, int height) {
    m_renderer->resize(width, height);
}

IRenderer& Engine::getRenderer() {
    return *m_renderer;
}

void Engine::playBgm(const std::string& soundName) {
    std::string fullPath = getResourceDir() + "/" + soundName;
    m_audio->play_bgm(fullPath.c_str());
}

const std::string& Engine::getResourceDir() const {
    return m_resourceDir;
}
} // namespace nyanchu