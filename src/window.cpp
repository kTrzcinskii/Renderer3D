//
// Created by Kacper Trzciński on 14.01.2025.
//

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "window.h"

namespace Renderer3D {
    Window::Window(const size_t width, const size_t height) : _glfwWindow(nullptr, nullptr)
    {
        glfwSetErrorCallback(glfwErrorCallback);
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
        glfwSetWindowUserPointer(_glfwWindow.get(), nullptr);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            spdlog::error("Failed to initialize GLAD");
        }

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

    void Window::LockCursor() const
    {
        glfwSetInputMode(_glfwWindow.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void Window::UnlockCursor() const
    {
        glfwSetInputMode(_glfwWindow.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void Window::SetUserPointer(void* userPointer) const
    {
        glfwSetWindowUserPointer(_glfwWindow.get(), userPointer);
    }

    void Window::SetWindowResizeCallback(const GLFWframebuffersizefun callback) const
    {
        glfwSetFramebufferSizeCallback(_glfwWindow.get(), callback);
    }

    void Window::SetCursorPositionCallback(const GLFWcursorposfun callback) const
    {
        glfwSetCursorPosCallback(_glfwWindow.get(), callback);
    }

    void Window::SetKeyCallback(const GLFWkeyfun callback) const
    {
        glfwSetKeyCallback(_glfwWindow.get(), callback);
    }

    bool Window::IsKeyPressed(const int key) const
    {
        return glfwGetKey(_glfwWindow.get(), key) == GLFW_TRUE;
    }

    void Window::Close() const
    {
        glfwSetWindowShouldClose(_glfwWindow.get(), true);
    }

    void Window::InitImGuiBackend() const
    {
        ImGui_ImplGlfw_InitForOpenGL(_glfwWindow.get(), true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
    }

    void Window::glfwErrorCallback(int error, const char* description)
    {
        spdlog::error("GLFW error {}: {}", error, description);
    }
} // Renderer3D