//
// Created by Kacper Trzciński on 14.01.2025.
//

#ifndef WINDOW_H
#define WINDOW_H

#include <memory>
#include <GLFW/glfw3.h>

namespace Renderer3D {

    class Window {
    public:
        explicit Window(size_t width = Window::INITIAL_WIDTH, size_t height = Window::INITIAL_HEIGHT);
        [[nodiscard]] bool ShouldClose() const;
        void SwapBuffers() const;
        void PollEvents() const;
        void LockCursor() const;
        void UnlockCursor() const;
        void SetUserPointer(void* userPointer) const;
        void SetWindowResizeCallback(GLFWframebuffersizefun callback) const;
        void SetCursorPositionCallback(GLFWcursorposfun callback) const;
        [[nodiscard]] bool IsKeyPressed(int key) const;
        void Close() const;
    private:
        std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> _glfwWindow;
        static constexpr size_t INITIAL_WIDTH = 1600;
        static constexpr size_t INITIAL_HEIGHT = 800;
    };

} // Renderer3D

#endif //WINDOW_H
