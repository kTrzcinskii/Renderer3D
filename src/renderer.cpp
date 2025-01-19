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
    Renderer::Renderer() : _window(Renderer::INITIAL_WIDTH, Renderer::INITIAL_HEIGHT), _freeMovingCamera(Renderer::INITIAL_WIDTH, Renderer::INITIAL_HEIGHT), _deferredShader(Renderer::INITIAL_WIDTH, Renderer::INITIAL_HEIGHT), _scene(std::make_unique<Scene>(Scene()))
    {
        // We store pointer to renderer inside window so we could easily set up callbacks
        _window.SetUserPointer(this);
        _window.SetWindowResizeCallback(ResizeCallback);
        _window.SetCursorPositionCallback(CursorPosCallback);
        _window.SetKeyCallback(KeyCallback);

        // Init controls
        _controls = std::make_unique<Controls>(_window);

        // Setup scene
        GeneratePointLightsForScene();
        SetupModelsForScene();
        SetupSkyboxesForScene();
    }

    void Renderer::Render()
    {
        glEnable(GL_DEPTH_TEST);

        _window.LockCursor();

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
            _scene->UpdateEntities(_deltaTime);
            _scene->RenderEntitiesToGeometryPass(_deferredShader.GetGeometryPassShader(), view, projection);

            // Bind back to default frame buffer
            _deferredShader.UnbindGBuffer();

            // Lighting pass - calculate lighting using data from geometry pass
            _deferredShader.GetLightingPassShader()->Activate();
            _deferredShader.BindGTextures();
            _scene->SetLightingPassShaderData(_deferredShader.GetLightingPassShader());
            _deferredShader.GetLightingPassShader()->SetUniform("cameraPos", _freeMovingCamera.GetPosition());
            _deferredShader.UpdateAmbientLevel(_controls->GetSceneMode());

            // Render quad with proper lighting from previous step
            _deferredShader.RenderQuad();

            // Copy depth buffer to be able to use forward rendering
            _deferredShader.CopyDepthBufferToDefaultBuffer();

            // Render additional effects using forward rendering
            _scene->RenderPointLightsForwardRendering(view, projection);
            RenderSkybox(view, projection);

            _window.PollEvents();

            // Draw controls
            _controls->Draw();

            _window.SwapBuffers();
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
        if (!_isCursorLocked)
        {
            return;
        }
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

    void Renderer::ProcessKeyCallback(const int key, const int action)
    {
        if (key == GLFW_KEY_L && action == GLFW_RELEASE)
        {
            if (_isCursorLocked)
            {
                _window.UnlockCursor();
                _isCursorLocked = false;
                spdlog::info("Cursor unlocked!");
            }
            else
            {
                _window.LockCursor();
                _isCursorLocked = true;
                spdlog::info("Cursor locked!");
            }
        }
    }

    void Renderer::RenderSkybox(const glm::mat4& view, const glm::mat4& projection) const
    {
        switch (_controls->GetSceneMode())
        {
        case SceneMode::Day:
            _scene->RenderDaySkyboxForwardRendering(view, projection);
            break;
        case SceneMode::Night:
            _scene->RenderNightSkyboxForwardRendering(view, projection);
            break;
        }
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

    void Renderer::KeyCallback(GLFWwindow* window, const int key, int scancode, const int action, int mods)
    {
        // ReSharper disable once CppTooWideScopeInitStatement
        const auto renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
        if (renderer == nullptr)
        {
            spdlog::error("Window has user pointer set to NULL");
            return;
        }
        renderer->ProcessKeyCallback(key, action);
    }

    void Renderer::GeneratePointLightsForScene() const
    {
        std::vector<PointLightSource> pointLightSources;
        srand(time(nullptr));
        for (unsigned int i = 0; i < Renderer::POINTS_LIGHTS_COUNT; i++)
        {
            // Random offsets
            const auto xPos = static_cast<float>(rand() % 100 / 100.0 * 30.0 - 15.0);
            const auto yPos = static_cast<float>(rand() % 100 / 100.0 * 5.0 + 0.1);
            const auto zPos = static_cast<float>(rand() % 100 / 100.0 * 30.0 - 15.0);
            const auto position = glm::vec3(xPos, yPos, zPos);
            // Random color
            const auto rColor = static_cast<float>(rand() % 100 / 200.0f + 0.5); // between 0.5 and 1.)
            const auto gColor = static_cast<float>(rand() % 100 / 200.0f + 0.5); // between 0.5 and 1.)
            const auto bColor = static_cast<float>(rand() % 100 / 200.0f + 0.5); // between 0.5 and 1.)
            const auto color = glm::vec3(rColor, gColor, bColor);
            pointLightSources.emplace_back(position, color);
        }
        _scene->UpdatePointLightContainer(std::make_unique<PointLightsContainer>(PointLightsContainer(pointLightSources)));
    }

    void Renderer::SetupModelsForScene() const
    {
        Entity backpack("../assets/models/backpack/backpack.obj", true);
        backpack.UpdatePosition(glm::vec3(0.0f, 2.0f, -3.0f));
        backpack.UpdateScale(glm::vec3(0.8f, 0.8f, 0.8f));
        _scene->AddEntity("backpack", backpack);

        Entity ufo("../assets/models/ufo/Low_poly_UFO.obj");
        ufo.UpdatePosition(glm::vec3(5.0f, 3.5f, 4.0f));
        ufo.UpdateScale(glm::vec3(0.09f, 0.09f, 0.09f));
        _scene->AddEntity("ufo", ufo);
        _scene->AddEntityUpdateFunction("ufo", [](Entity& entity, const float deltaTime)
        {
            entity.UpdateRotationY(entity.GetRotationY() + deltaTime * 25.0f);
        });

        Entity cottage("../assets/models/cottage/Cottage_FREE.obj");
        cottage.UpdatePosition(glm::vec3(14.0f, 0.0f, 2.0f));
        cottage.UpdateRotationY(90.0f);
        _scene->AddEntity("cottage", cottage);

        Entity farmHouse("../assets/models/farm_house/farmhouse_obj.obj");
        farmHouse.UpdatePosition(glm::vec3(-8.0f, 0.0f, 2.0f));
        farmHouse.UpdateScale(glm::vec3(0.2f, 0.2f, 0.2f));
        farmHouse.UpdateRotationY(-90.0f);
        _scene->AddEntity("farmHouse", farmHouse);
    }

    void Renderer::SetupSkyboxesForScene() const
    {
        const auto& skyboxShader = std::make_shared<Shader>("../assets/shaders/skybox_vertex.glsl", "../assets/shaders/skybox_fragment.glsl");
        _scene->UpdateNightSkybox(std::make_unique<Skybox>("../assets/cubemaps/night/px.jpg", "../assets/cubemaps/night/nx.jpg", "../assets/cubemaps/night/py.jpg", "../assets/cubemaps/night/ny.jpg", "../assets/cubemaps/night/pz.jpg", "../assets/cubemaps/night/nz.jpg", skyboxShader));
        _scene->UpdateDaySkybox(std::make_unique<Skybox>("../assets/cubemaps/day/Daylight Box_Right.bmp", "../assets/cubemaps/day/Daylight Box_Left.bmp", "../assets/cubemaps/day/Daylight Box_Top.bmp", "../assets/cubemaps/day/Daylight Box_Bottom.bmp", "../assets/cubemaps/day/Daylight Box_Front.bmp", "../assets/cubemaps/day/Daylight Box_Back.bmp", skyboxShader));
    }
} // Renderer3D