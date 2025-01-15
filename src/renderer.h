//
// Created by Kacper Trzciński on 14.01.2025.
//

#ifndef RENDERER_H
#define RENDERER_H

#include "window.h"
#include "camera.h"

namespace Renderer3D {

    class Renderer {
    public:
        Renderer();
        void Render();
    private:
        // Renderer data
        float _lastFrameTime = 0.0;
        float _deltaTime = 0.0;
        bool _firstMouseMove = true;
        float _mouseXPos = Renderer::INITIAL_WIDTH / 2.0f;
        float _mouseYPos = Renderer::INITIAL_HEIGHT / 2.0f;;

        // Objects
        Window _window;
        Camera _freeMovingCamera;

        // Actions
        void ProcessWindowResize(int width, int height);
        void ProcessInput();
        void ProcessMouseMovement(double xPos, double yPos);

        // Window callbacks
        static void ResizeCallback(GLFWwindow* window, int width, int height);
        static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);

        // Consts
        static constexpr size_t INITIAL_WIDTH = 1600;
        static constexpr size_t INITIAL_HEIGHT = 800;
    };

} // Renderer3D

#endif //RENDERER_H
