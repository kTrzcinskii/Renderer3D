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

        // Scene mode
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
} // Renderer3D