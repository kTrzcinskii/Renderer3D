//
// Created by Kacper Trzciński on 19.01.2025.
//

#include <imgui.h>
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

    void Controls::Draw()
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

        // Camera flashlight
        ImGui::Spacing();
        ImGui::Text("Camera flashlight");
        ImGui::Checkbox("Enable", &_useCameraFlashlight);

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

    float Controls::GetUseCameraFlashlight() const
    {
        return _useCameraFlashlight;
    }
} // Renderer3D