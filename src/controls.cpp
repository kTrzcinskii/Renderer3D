//
// Created by Kacper Trzciński on 19.01.2025.
//

#include <format>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "controls.h"

namespace Renderer3D {
    Controls::Controls(const Window& window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        window.InitImGuiBackend();
        ImGui::StyleColorsDark();
    }

    Controls::~Controls()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Controls::Draw(const std::unique_ptr<PointLightsContainer>& pointLightsContainer)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Auto minimal size that is enough for whole content
        ImGui::SetNextWindowSize(ImVec2(0, 0));

        ImGui::Begin("Controls");

        // Information about locking cursor
        ImGui::Text("Press 'L' to lock/unlock cursor.");
        ImGui::Spacing();

        // Projection type
        ImGui::Spacing();
        ImGui::Text("Projection type");
        if (ImGui::RadioButton("Perspective", _projectionType == ProjectionType::PERSPECTIVE))
        {
            _projectionType = ProjectionType::PERSPECTIVE;
        }
        if (ImGui::RadioButton("Orthographic", _projectionType == ProjectionType::ORTHOGRAPHIC))
        {
            _projectionType = ProjectionType::ORTHOGRAPHIC;
        }

        // Camera type
        ImGui::Spacing();
        ImGui::Text("Camera type");
        if (ImGui::RadioButton("Moving", _cameraType == CameraType::MOVING))
        {
            _cameraType = CameraType::MOVING;
        }
        if (ImGui::RadioButton("Static", _cameraType == CameraType::STATIC))
        {
            _cameraType = CameraType::STATIC;
        }
        if (ImGui::RadioButton("Following object", _cameraType == CameraType::OBJECT_FOLLOWING))
        {
            _cameraType = CameraType::OBJECT_FOLLOWING;
        }
        if (ImGui::RadioButton("Object third person", _cameraType == CameraType::OBJECT_THIRD_PERSON))
        {
            _cameraType = CameraType::OBJECT_THIRD_PERSON;
        }

        // Camera flashlight
        if (_cameraType == CameraType::MOVING && _projectionType == ProjectionType::PERSPECTIVE)
        {
            ImGui::Spacing();
            ImGui::Text("Camera flashlight");
            ImGui::Checkbox("Enable", &_useCameraFlashlight);
        }

        // Ufo for following/third person camera
        if (_cameraType == CameraType::OBJECT_FOLLOWING || _cameraType == CameraType::OBJECT_THIRD_PERSON)
        {
            ImGui::Spacing();
            ImGui::Text("Select ufo");
            if (ImGui::RadioButton("1", _selectedUfoIndex == 0))
            {
                _selectedUfoIndex = 0;
            }
            ImGui::SameLine();

            if (ImGui::RadioButton("2", _selectedUfoIndex == 1))
            {
                _selectedUfoIndex = 1;
            }
            ImGui::SameLine();

            if (ImGui::RadioButton("3", _selectedUfoIndex == 2))
            {
                _selectedUfoIndex = 2;
            }
            ImGui::SameLine();

            if (ImGui::RadioButton("4", _selectedUfoIndex == 3))
            {
                _selectedUfoIndex = 3;
            }
        }

        // Ufo flashlight direction
        ImGui::Spacing();
        ImGui::Text("UFOs flashlights");
        ImGui::SliderFloat("Direction X", &_ufosFlashlightDirection.x, -80.0f, 80.0f, "%.0f");
        ImGui::SliderFloat("Direction Z", &_ufosFlashlightDirection.z, -80.0f, 80.0f, "%.0f");

        // Point lights
        ImGui::Spacing();
        ImGui::Text("Point lights");
        const auto pointLightsCount = std::format("Count: {}", pointLightsContainer->GetPointLightCount());
        ImGui::Text(pointLightsCount.c_str());
        // Adding
        if (!pointLightsContainer->CanAddPointLight())
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        bool changedThisFrame = false;
        if (ImGui::Button("Create single"))
        {
            pointLightsContainer->AddPointLight(PointLightSource::GenerateRandom(MIN_X, MAX_X, MIN_Y, MAX_Y, MIN_Z, MAX_Z));
            changedThisFrame = true;
        }
        else if (ImGui::Button("Create max capacity"))
        {
            while (pointLightsContainer->CanAddPointLight())
            {
                pointLightsContainer->AddPointLight(PointLightSource::GenerateRandom(MIN_X, MAX_X, MIN_Y, MAX_Y, MIN_Z, MAX_Z));
            }
            changedThisFrame = true;
        }
        if (!pointLightsContainer->CanAddPointLight() && !changedThisFrame)
        {
            ImGui::PopStyleVar();
            ImGui::PopItemFlag();
        }
        // Removing
        if (!pointLightsContainer->CanRemovePointLight())
        {
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
        }
        changedThisFrame = false;
        if (ImGui::Button("Remove single"))
        {
            pointLightsContainer->RemovePointLight(rand() % pointLightsContainer->GetPointLightCount());
            changedThisFrame = true;
        }
        else if (ImGui::Button("Remove all"))
        {
            while (pointLightsContainer->CanRemovePointLight())
            {
                pointLightsContainer->RemovePointLight(rand() % pointLightsContainer->GetPointLightCount());
            }
            changedThisFrame = true;
        }
        if (!pointLightsContainer->CanRemovePointLight() && !changedThisFrame)
        {
            ImGui::PopStyleVar();
            ImGui::PopItemFlag();
        }

        // Scene mode
        ImGui::Spacing();
        ImGui::Text("Scene mode:");
        if (ImGui::RadioButton("Day", _sceneMode == SceneMode::Day)) {
            _sceneMode = SceneMode::Day;
        }
        if (ImGui::RadioButton("Night", _sceneMode == SceneMode::Night)) {
            _sceneMode = SceneMode::Night;
        }
        if (ImGui::RadioButton("Fog", _sceneMode == SceneMode::Fog)) {
            _sceneMode = SceneMode::Fog;
        }

        // Fog strength
        ImGui::Spacing();
        if (_sceneMode == SceneMode::Fog)
        {
            ImGui::SliderFloat("Fog strength", &_fogStrength, 0.0f, 99.0f, "%.0f");
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Controls::UpdateCanAddPointLight(const bool canAdd)
    {
        _canAddPointLight = canAdd;
    }

    SceneMode Controls::GetSceneMode() const
    {
        return _sceneMode;
    }

    float Controls::GetFogStrength() const
    {
        return _fogStrength;
    }

    bool Controls::IsFog() const
    {
        return _sceneMode == SceneMode::Fog;
    }

    ProjectionType Controls::GetProjectionType() const
    {
        return _projectionType;
    }

    bool Controls::IsOrthographic() const
    {
        return _projectionType == ProjectionType::ORTHOGRAPHIC;
    }

    bool Controls::GetUseCameraFlashlight() const
    {
        return _useCameraFlashlight;
    }

    FlashlightDirections Controls::GetUfosFlashlightDirection() const
    {
        return _ufosFlashlightDirection;
    }

    CameraType Controls::GetCameraType() const
    {
        return _cameraType;
    }

    size_t Controls::GetSelectedUfoIndex() const
    {
        return _selectedUfoIndex;
    }
} // Renderer3D