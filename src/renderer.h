//
// Created by Kacper Trzciński on 14.01.2025.
//

#ifndef RENDERER_H
#define RENDERER_H

#include "window.h"

namespace Renderer3D {

    class Renderer {
    public:
        Renderer();
        void Render();
    private:
        Window _window;
        float _lastFrameTime = 0.0;

        // Actions
        void ProcessWindowResize(int width, int height);
        void ProcessInput();

        // Window callbacks
        static void ResizeCallback(GLFWwindow* window, int width, int height);
    };

} // Renderer3D

#endif //RENDERER_H
