//
// Created by Kacper Trzciński on 14.01.2025.
//

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderer.h"

namespace Renderer3D {
    Renderer::Renderer()
    {
        // We store pointer to renderer inside window so we could easily setup callbacks
        _window.SetUserPointer(this);
        _window.SetWindowResizeCallback(ResizeCallback);
    }

    void Renderer::Render()
    {
        while (!_window.ShouldClose())
        {
            // Handle input
            ProcessInput();

            // Render
            glClearColor(0.2, 0.5f, 0.3f, 0.4f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Double buffer and events
            _window.SwapBuffers();
            _window.PollEvents();
        }
    }

    void Renderer::ProcessWindowResize(const int width, const int height)
    {
        spdlog::info("Window resized: {}x{}", width, height);
        glViewport(0, 0, width, height);
    }

    void Renderer::ProcessInput()
    {
        if (_window.IsKeyPressed(GLFW_KEY_ESCAPE))
        {
            _window.Close();
        }

        if (_window.IsKeyPressed(GLFW_KEY_W))
        {
            spdlog::info("Key 'W' pressed!");
        }
    }

    void Renderer::ResizeCallback(GLFWwindow* window, const int width, const int height)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        if (renderer == nullptr)
        {
            spdlog::error("Window has user pointer set to NULL");
            return;
        }
        renderer->ProcessWindowResize(width, height);
    }

} // Renderer3D