//
// Created by Kacper Trzciński on 14.01.2025.
//

#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "camera.h"
#include "controls.h"
#include "deferred_shaderer.h"
#include "models_manager.h"
#include "scene.h"

namespace Renderer3D {

    class Renderer {
    public:
        Renderer();
        void Render();
    private:
        // Controls state
        bool _isCursorLocked = true;

        // Renderer data
        float _lastFrameTime = 0.0;
        float _deltaTime = 0.0;
        bool _firstMouseMove = true;
        float _mouseXPos = Renderer::INITIAL_WIDTH / 2.0f;
        float _mouseYPos = Renderer::INITIAL_HEIGHT / 2.0f;

        // Objects
        Window _window;
        Camera _freeMovingCamera;
        DeferredShaderer _deferredShader;
        std::unique_ptr<Scene> _scene = nullptr;
        std::unique_ptr<Controls> _controls = nullptr;
        std::unique_ptr<ModelsManager> _modelsManager = std::make_unique<ModelsManager>();

        // Actions
        void ProcessWindowResize(int width, int height);
        void ProcessInput();
        void ProcessMouseMovement(double xPos, double yPos);
        void ProcessKeyCallback(int key, int action);
        void RenderSkybox(const glm::mat4& view, const glm::mat4& projection) const;

        // Window callbacks
        static void ResizeCallback(GLFWwindow* window, int width, int height);
        static void CursorPosCallback(GLFWwindow* window, double xPos, double yPos);
        static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

        // Helpers
        void GeneratePointLightsForScene() const;
        void SetupModelsForScene() const;
        void SetupSkyboxesForScene() const;

        // Consts
        static constexpr size_t INITIAL_WIDTH = 1600;
        static constexpr size_t INITIAL_HEIGHT = 800;
        static constexpr size_t POINTS_LIGHTS_COUNT = 256;
    };

} // Renderer3D

#endif //RENDERER_H
