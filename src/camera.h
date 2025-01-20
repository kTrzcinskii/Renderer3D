//
// Created by Kacper Trzciński on 13.01.2025.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Renderer3D {

    enum class CameraMovementDirection
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    struct RotateDiff
    {
        float xOffset;
        float yOffset;
    };

    // TODO: add switching to orthogonal projection
    class Camera {
    public:
        Camera(float screenWidth, float screenHeight, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float near = Camera::DEFAULT_NEAR, float far = Camera::DEFAULT_FAR, float yaw = Camera::DEFAULT_YAW, float pitch = Camera::DEFAULT_PITCH);
        Camera(float screenWidth, float screenHeight, float posX, float posY, float posZ, float upX, float upY, float upZ, float near, float far, float yaw, float pitch);
        // Getters
        [[nodiscard]] glm::vec3 GetPosition() const;
        [[nodiscard]] glm::mat4 GetViewMatrix() const;
        [[nodiscard]] glm::mat4 GetProjectionMatrix() const;
        [[nodiscard]] float GetFarZ() const;
        // Actions
        void Move(CameraMovementDirection direction, float deltaTime);
        void Rotate(RotateDiff diff, bool constraintPitch = true);
        void Zoom(float offset);
        void UpdateScreenSize(float screenWidth, float screenHeight);
    private:
        // Camera attributes
        glm::vec3 _position{};
        glm::vec3 _front{};
        glm::vec3 _up{};
        glm::vec3 _right{};
        glm::vec3 _worldUp{};
        float _screenWidth;
        float _screenHeight;
        float _near;
        float _far;
        // Euler angles
        float _yaw;
        float _pitch;
        // Camera options
        float _movementSpeed;
        float _mouseSensitivity;
        float _zoom;
        // Default values
        static constexpr float DEFAULT_NEAR = 0.1f;
        static constexpr float DEFAULT_FAR = 100.0f;
        static constexpr float DEFAULT_YAW = -90.0f;
        static constexpr float DEFAULT_PITCH = 0.0f;
        static constexpr float DEFAULT_SPEED = 2.5f;
        static constexpr float DEFAULT_SENSITIVITY = 0.1f;
        static constexpr float DEFAULT_ZOOM = 45.0f;

        void UpdateCameraVectors();
    };

} // Renderer3D

#endif //CAMERA_H
