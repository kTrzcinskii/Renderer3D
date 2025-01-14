//
// Created by Kacper Trzciński on 13.01.2025.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include "window.h"

int main()
{
    const auto window = Renderer3D::Window();

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        spdlog::error("Failed to initialize GLAD");
        return -1;
    }

    while (!window.ShouldClose())
    {
        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}