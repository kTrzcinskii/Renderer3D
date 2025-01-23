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

    struct FlashlightDirections
    {
        float x;
        float z;
    };

    enum class CameraType
    {
        MOVING = 0,
        STATIC,
        OBJECT_FOLLOWING,
        OBJECT_THIRD_PERSON,
    };

    constexpr size_t CAMERA_TYPE_COUNT = 4;

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
        [[nodiscard]] bool GetUseCameraFlashlight() const;
        [[nodiscard]] FlashlightDirections GetUfosFlashlightDirection() const;
        [[nodiscard]] CameraType GetCameraType() const;
        [[nodiscard]] size_t GetSelectedUfoIndex() const;
    private:
        SceneMode _sceneMode = SceneMode::Day;
        float _fogStrength = 0.0f;
        ProjectionType _projectionType = ProjectionType::PERSPECTIVE;
        bool _useCameraFlashlight = false;
        FlashlightDirections _ufosFlashlightDirection = {0.0f, 0.0f};
        CameraType _cameraType = CameraType::MOVING;
        size_t _selectedUfoIndex = 0;
    };

} // Renderer3D

#endif //CONTROLS_H
