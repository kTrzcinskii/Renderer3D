//
// Created by Kacper Trzciński on 13.01.2025.
//

#include "window.h"

int main()
{
    const auto window = Renderer3D::Window();

    while (!window.ShouldClose())
    {
        window.SwapBuffers();
        window.PollEvents();
    }

    return 0;
}