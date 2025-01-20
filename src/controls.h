//
// Created by Kacper Trzciński on 19.01.2025.
//

#ifndef CONTROLS_H
#define CONTROLS_H

#include "window.h"

namespace Renderer3D {

    enum class SceneMode
    {
        Day,
        Night,
        Fog
    };

    enum class ProjectionType
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    class Controls {
    public:
        explicit Controls(const Window& window);
        ~Controls();
        void Draw();
        [[nodiscard]] SceneMode GetSceneMode() const;
        [[nodiscard]] float GetFogStrength() const;
        [[nodiscard]] bool IsFog() const;
        [[nodiscard]] ProjectionType GetProjectionType() const;
        [[nodiscard]] bool IsOrthographic() const;
    private:
        SceneMode _sceneMode = SceneMode::Day;
        float _fogStrength = 0.0f;
        ProjectionType _projectionType = ProjectionType::PERSPECTIVE;
    };

} // Renderer3D

#endif //CONTROLS_H
