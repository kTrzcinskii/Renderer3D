//
// Created by Kacper Trzciński on 14.01.2025.
//

#include <spdlog/spdlog.h>

#include "window.h"

namespace Renderer3D {
    Window::Window(const size_t width, const size_t height) : _glfwWindow(nullptr, nullptr)
    {
        if (glfwInit() == GLFW_FALSE)
        {
            spdlog::error("Failed to initialize glfw");
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        _glfwWindow = std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>(
            glfwCreateWindow(
                static_cast<int>(width),
                static_cast<int>(height),
                "Renderer3D",
                nullptr,
                nullptr
            ),
            glfwDestroyWindow
        );

        if (!_glfwWindow)
        {
            spdlog::error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(_glfwWindow.get());
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(_glfwWindow.get());
    }

    void Window::SwapBuffers() const
    {
        glfwSwapBuffers(_glfwWindow.get());
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void Window::PollEvents() const // NOLINT(*-convert-member-functions-to-static)
    {
        glfwPollEvents();
    }
} // Renderer3D