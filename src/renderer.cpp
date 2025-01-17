//
// Created by Kacper Trzciński on 14.01.2025.
//

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include "renderer.h"
#include "shader.h"
#include "model.h"
#include "point_light_source.h"

namespace Renderer3D {
    Renderer::Renderer() : _window(Renderer::INITIAL_WIDTH, Renderer::INITIAL_HEIGHT), _freeMovingCamera(Renderer::INITIAL_WIDTH, Renderer::INITIAL_HEIGHT), _deferredShader(Renderer::INITIAL_WIDTH, Renderer::INITIAL_HEIGHT)
    {
        // We store pointer to renderer inside window so we could easily set up callbacks
        _window.SetUserPointer(this);
        _window.SetWindowResizeCallback(ResizeCallback);
        _window.SetCursorPositionCallback(CursorPosCallback);
    }

    // TODO: move to PointLightSource class
    // renderSphere() renders a 3D sphere in NDC.
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int sphereVBO = 0;
unsigned int sphereEBO = 0; // For indexed rendering
void renderSphere()
{
    const unsigned int X_SEGMENTS = 64; // Number of segments along longitude
    const unsigned int Y_SEGMENTS = 64; // Number of segments along latitude
    const float PI = 3.14159265359f;

    static std::vector<float> vertices;
    static std::vector<unsigned int> indices;

    if (sphereVAO == 0)
    {
        // Generate vertices
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / X_SEGMENTS;
                float ySegment = (float)y / Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                // Vertex position
                vertices.push_back(xPos);
                vertices.push_back(yPos);
                vertices.push_back(zPos);

                // Normal (same as position for unit sphere)
                vertices.push_back(xPos);
                vertices.push_back(yPos);
                vertices.push_back(zPos);

                // Texture coordinates
                vertices.push_back(xSegment);
                vertices.push_back(ySegment);
            }
        }

        // Generate indices
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x < X_SEGMENTS; ++x)
            {
                unsigned int first = (y * (X_SEGMENTS + 1)) + x;
                unsigned int second = first + X_SEGMENTS + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        // Generate and bind buffers
        glGenVertexArrays(1, &sphereVAO);
        glGenBuffers(1, &sphereVBO);
        glGenBuffers(1, &sphereEBO);

        glBindVertexArray(sphereVAO);

        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Set vertex attribute pointers
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Render sphere
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

    void Renderer::Render()
    {
        glEnable(GL_DEPTH_TEST);

        // TODO: remove, only for testing
        _window.LockCursor();
        const Shader lightSourceShader("../assets/shaders/light_source_vertex.glsl", "../assets/shaders/light_source_fragment.glsl");

        const Model backpack("../assets/models/backpack/backpack.obj", true);
        const Model ufo("../assets/models/ufo/Low_poly_UFO.obj");
        const Model cottage("../assets/models/cottage/Cottage_FREE.obj");

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
            auto backpackModelMatrix = glm::mat4(1.0f);
            backpackModelMatrix = glm::translate(backpackModelMatrix, glm::vec3(-0.0f, -1.0f, -3.0f));
            backpackModelMatrix = glm::scale(backpackModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
            _deferredShader.GetGeometryPassShader()->SetUniform("model", backpackModelMatrix);
            backpack.Draw(_deferredShader.GetGeometryPassShader());

            // Render ufo to gBuffer
            auto ufoModelMatrix = glm::mat4(1.0f);
            ufoModelMatrix = glm::translate(ufoModelMatrix, glm::vec3(5.0f, 1.0f, 4.0f));
            ufoModelMatrix = glm::scale(ufoModelMatrix, glm::vec3(0.09f, 0.09f, 0.09f));
            _deferredShader.GetGeometryPassShader()->SetUniform("model", ufoModelMatrix);
            ufo.Draw(_deferredShader.GetGeometryPassShader());

            // Render cottage to gBuffer
            auto cottageModelMatrix = glm::mat4(1.0f);
            cottageModelMatrix = glm::translate(cottageModelMatrix, glm::vec3(14.0f, -2.0f, 2.0f));
            _deferredShader.GetGeometryPassShader()->SetUniform("model", cottageModelMatrix);
            cottage.Draw(_deferredShader.GetGeometryPassShader());

            // Bind back to default frame buffer
            _deferredShader.UnbindGBuffer();

            // Lighting pass - calculate lighting using data from geometry pass
            _deferredShader.GetLightingPassShader()->Activate();
            _deferredShader.BindGTextures();

            // Set point lights data
            _deferredShader.GetLightingPassShader()->SetUniform("nrPointLights", static_cast<int>(pointLightSources.size()));
            for (size_t i = 0; i < pointLightSources.size(); i++)
            {
                pointLightSources[i].SetUniforms(_deferredShader.GetLightingPassShader(), i);
            }

            _deferredShader.GetLightingPassShader()->SetUniform("cameraPos", _freeMovingCamera.GetPosition());

            // Render quad with proper lighting from previous step
            _deferredShader.RenderQuad();

            // Copy depth buffer to be able to use forward rendering
            _deferredShader.CopyDepthBufferToDefaultBuffer();

            // Render light sources using forward rendering
            lightSourceShader.Activate();
            lightSourceShader.SetUniform("view", view);
            lightSourceShader.SetUniform("projection", projection);
            for (size_t i = 0; i < pointLightSources.size(); i++)
            {
                auto model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightSources[i].GetPosition());
                model = glm::scale(model, glm::vec3(0.125f));
                lightSourceShader.SetUniform("model", model);
                lightSourceShader.SetUniform("lightColor", pointLightSources[i].GetColor());
                renderSphere();
            }

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