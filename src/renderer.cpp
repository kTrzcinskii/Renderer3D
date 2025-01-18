//
// Created by Kacper Trzciński on 14.01.2025.
//

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include "renderer.h"

#include "entity.h"
#include "shader.h"
#include "point_light_source.h"

namespace Renderer3D {
    Renderer::Renderer() : _window(Renderer::INITIAL_WIDTH, Renderer::INITIAL_HEIGHT), _freeMovingCamera(Renderer::INITIAL_WIDTH, Renderer::INITIAL_HEIGHT), _deferredShader(Renderer::INITIAL_WIDTH, Renderer::INITIAL_HEIGHT)
    {
        // We store pointer to renderer inside window so we could easily set up callbacks
        _window.SetUserPointer(this);
        _window.SetWindowResizeCallback(ResizeCallback);
        _window.SetCursorPositionCallback(CursorPosCallback);
        // TODO: move to scene class probably?
        const unsigned int NR_LIGHTS = 256;
        std::vector<PointLightSource> pointLightSources;
        srand(time(nullptr));
        for (unsigned int i = 0; i < NR_LIGHTS; i++)
        {
            // calculate slightly random offsets
            float xPos = static_cast<float>(((rand() % 100) / 100.0) * 16.0 - 3.0);
            float yPos = static_cast<float>(((rand() % 100) / 100.0) * 16.0 - 4.0);
            float zPos = static_cast<float>(((rand() % 100) / 100.0) * 16.0 - 3.0);
            const auto position = glm::vec3(xPos, yPos, zPos);
            // also calculate random color
            float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.)
            float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.)
            float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.)
            const auto color = glm::vec3(rColor, gColor, bColor);
            pointLightSources.push_back(PointLightSource(position, color));
        }
        _pointLightsContainer = std::make_unique<PointLightsContainer>(PointLightsContainer(pointLightSources));
    }

    void Renderer::Render()
    {
        glEnable(GL_DEPTH_TEST);

        // TODO: remove, only for testing
        _window.LockCursor();

        Entity backpack("../assets/models/backpack/backpack.obj", true);
        backpack.UpdatePosition(glm::vec3(-0.0f, -1.0f, -3.0f));
        backpack.UpdateScale(glm::vec3(0.8f, 0.8f, 0.8f));

        Entity ufo("../assets/models/ufo/Low_poly_UFO.obj");
        ufo.UpdatePosition(glm::vec3(5.0f, 1.0f, 4.0f));
        ufo.UpdateScale(glm::vec3(0.09f, 0.09f, 0.09f));

        Entity cottage("../assets/models/cottage/Cottage_FREE.obj");
        cottage.UpdatePosition(glm::vec3(14.0f, -2.0f, 2.0f));
        cottage.UpdateRotationY(90.0f);

        // TODO: move to scene class probably?
        const unsigned int NR_LIGHTS = 256;
        std::vector<PointLightSource> pointLightSources;
        srand(time(nullptr));
        for (unsigned int i = 0; i < NR_LIGHTS; i++)
        {
            // calculate slightly random offsets
            float xPos = static_cast<float>(((rand() % 100) / 100.0) * 16.0 - 3.0);
            float yPos = static_cast<float>(((rand() % 100) / 100.0) * 16.0 - 4.0);
            float zPos = static_cast<float>(((rand() % 100) / 100.0) * 16.0 - 3.0);
            const auto position = glm::vec3(xPos, yPos, zPos);
            // also calculate random color
            float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.)
            float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.)
            float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.)
            const auto color = glm::vec3(rColor, gColor, bColor);
            pointLightSources.push_back(PointLightSource(position, color));
        }

        while (!_window.ShouldClose())
        {
            // Delta time
            const auto currentTime = static_cast<float>(glfwGetTime());
            _deltaTime = currentTime - _lastFrameTime;
            _lastFrameTime = currentTime;

            // Handle input
            ProcessInput();

            // Render
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Camera matrices
            const auto projection = _freeMovingCamera.GetProjectionMatrix();
            const auto view = _freeMovingCamera.GetViewMatrix();

            // Geometry pass - render data into gBuffer
            _deferredShader.BindGBuffer();
            _deferredShader.GetGeometryPassShader()->Activate();
            _deferredShader.GetGeometryPassShader()->SetUniform("projection", projection);
            _deferredShader.GetGeometryPassShader()->SetUniform("view", view);

            // Render backpack to gBuffer
            backpack.Draw(_deferredShader.GetGeometryPassShader());

            // Render ufo to gBuffer
            ufo.UpdateRotationY(ufo.GetRotationY() + _deltaTime * 25.0f);
            ufo.Draw(_deferredShader.GetGeometryPassShader());

            // Render cottage to gBuffer
            cottage.Draw(_deferredShader.GetGeometryPassShader());

            // Bind back to default frame buffer
            _deferredShader.UnbindGBuffer();

            // Lighting pass - calculate lighting using data from geometry pass
            _deferredShader.GetLightingPassShader()->Activate();
            _deferredShader.BindGTextures();

            _pointLightsContainer->SetLightingPassPointLightsData(_deferredShader.GetLightingPassShader());

            _deferredShader.GetLightingPassShader()->SetUniform("cameraPos", _freeMovingCamera.GetPosition());

            // Render quad with proper lighting from previous step
            _deferredShader.RenderQuad();

            // Copy depth buffer to be able to use forward rendering
            _deferredShader.CopyDepthBufferToDefaultBuffer();

            // Render point light sources using forward rendering
            _pointLightsContainer->RenderPointLights(view, projection);

            // Double buffer and events
            _window.SwapBuffers();
            _window.PollEvents();
        }
    }

    void Renderer::ProcessWindowResize(const int width, const int height)
    {
        spdlog::info("Window resized: {}x{}", width, height);
        glViewport(0, 0, width, height);
        _freeMovingCamera.UpdateScreenSize(static_cast<float>(width), static_cast<float>(height));
        _deferredShader.Resize(width, height);
    }

    void Renderer::ProcessInput()
    {
        if (_window.IsKeyPressed(GLFW_KEY_ESCAPE))
        {
            spdlog::info("Key 'ESC' pressed!");
            _window.Close();
        }

        if (_window.IsKeyPressed(GLFW_KEY_W))
        {
            spdlog::info("Key 'W' pressed!");
            _freeMovingCamera.Move(CameraMovementDirection::FORWARD, _deltaTime);
        }

        if (_window.IsKeyPressed(GLFW_KEY_S))
        {
            spdlog::info("Key 'S' pressed!");
            _freeMovingCamera.Move(CameraMovementDirection::BACKWARD, _deltaTime);
        }

        if (_window.IsKeyPressed(GLFW_KEY_A))
        {
            spdlog::info("Key 'A' pressed!");
            _freeMovingCamera.Move(CameraMovementDirection::LEFT, _deltaTime);
        }

        if (_window.IsKeyPressed(GLFW_KEY_D))
        {
            spdlog::info("Key 'D' pressed!");
            _freeMovingCamera.Move(CameraMovementDirection::RIGHT, _deltaTime);
        }
    }

    void Renderer::ProcessMouseMovement(double xPos, double yPos)
    {
        spdlog::info("Mouse moved: {}x{}", xPos, yPos);
        const auto x = static_cast<float>(xPos);
        const auto y = static_cast<float>(yPos);
        if (_firstMouseMove)
        {
            _mouseXPos = x;
            _mouseYPos = y;
            _firstMouseMove = false;
        }

        const auto xOffset = x - _mouseXPos;
        const auto yOffset = _mouseYPos - y;
        _mouseXPos = x;
        _mouseYPos = y;

        _freeMovingCamera.Rotate({xOffset, yOffset});
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

    void Renderer::CursorPosCallback(GLFWwindow* window, double xPos, double yPos)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        if (renderer == nullptr)
        {
            spdlog::error("Window has user pointer set to NULL");
            return;
        }
        renderer->ProcessMouseMovement(xPos, yPos);
    }
} // Renderer3D