//
// Created by Kacper Trzciński on 13.01.2025.
//

#include <spdlog/spdlog.h>

#include "camera.h"

namespace Renderer3D {
    Camera::Camera(float screenWidth, float screenHeight, glm::vec3 position, glm::vec3 up, float near, float far, float yaw, float pitch)
    {
        _screenWidth = screenWidth;
        _screenHeight = screenHeight;
        _front = glm::vec3(0.0f, 0.0f, -1.0f);
        _movementSpeed = Camera::DEFAULT_SPEED;
        _mouseSensitivity = Camera::DEFAULT_SENSITIVITY;
        _zoom = Camera::DEFAULT_ZOOM;
        _position = position;
        _worldUp = up;
        _near = near;
        _far = far;
        _yaw = yaw;
        _pitch = pitch;
        UpdateCameraVectors();
    }

    Camera::Camera(float screenWidth, float screenHeight, float posX, float posY, float posZ, float upX, float upY, float upZ, float near, float far, float yaw, float pitch)
    {
        _screenWidth = screenWidth;
        _screenHeight = screenHeight;
        _front = glm::vec3(0.0f, 0.0f, -1.0f);
        _movementSpeed = Camera::DEFAULT_SPEED;
        _mouseSensitivity = Camera::DEFAULT_SENSITIVITY;
        _zoom = Camera::DEFAULT_ZOOM;
        _position = glm::vec3(posX, posY, posZ);
        _worldUp = glm::vec3(upX, upY, upZ);
        _near = near;
        _far = far;
        _yaw = yaw;
        _pitch = pitch;
        UpdateCameraVectors();
    }

    glm::vec3 Camera::GetPosition() const
    {
        return _position;
    }

    glm::mat4 Camera::GetViewMatrix() const
    {
        return glm::lookAt(_position, _position + _front, _up);
    }

    glm::mat4 Camera::GetProjectionMatrix() const
    {
        const auto aspect = _screenWidth / _screenHeight;
        switch (_projectionType)
        {
        case ProjectionType::PERSPECTIVE:
            return glm::perspective(glm::radians(_zoom), aspect, _near, _far);
        case ProjectionType::ORTHOGRAPHIC:
            return glm::ortho(-_screenWidth / ORTHO_FACTOR, _screenWidth / ORTHO_FACTOR, -_screenHeight / ORTHO_FACTOR, _screenHeight / ORTHO_FACTOR, _near, _far);
        };
        return glm::perspective(glm::radians(_zoom), aspect, _near, _far);
    }

    float Camera::GetFarZ() const
    {
        return _far;
    }

    glm::vec3 Camera::GetFront() const
    {
        return _front;
    }

    void Camera::Move(const CameraMovementDirection direction, const float deltaTime)
    {
        const auto velocity = _movementSpeed * deltaTime;
        switch (direction)
        {
        case CameraMovementDirection::FORWARD:
            _position += _front * velocity;
            break;
        case CameraMovementDirection::BACKWARD:
            _position -= _front * velocity;
            break;
        case CameraMovementDirection::LEFT:
            _position -= _right * velocity;
            break;
        case CameraMovementDirection::RIGHT:
            _position += _right * velocity;
            break;
        case CameraMovementDirection::UP:
            _position += _up * velocity;
            break;
        case CameraMovementDirection::DOWN:
            _position -= _up * velocity;
            break;
        }
        if (_flashlight != nullptr)
        {
            _flashlight->UpdatePosition(_position);
        }
    }

    void Camera::SetPosition(const glm::vec3 position)
    {
        _position = position;
        if (_flashlight != nullptr)
        {
            _flashlight->UpdatePosition(_position);
        }
    }

    void Camera::Rotate(const RotateDiff diff, const bool constraintPitch)
    {
        _yaw += diff.xOffset * _mouseSensitivity;
        _pitch += diff.yOffset * _mouseSensitivity;

        if (constraintPitch)
        {
            if (_pitch > 89.0f)
            {
                _pitch = 89.0f;
            }
            else if (_pitch < -89.0f)
            {
                _pitch = -89.0f;
            }
        }

        spdlog::info("Rotating camera: pitch - {}, yaw - {}", _pitch, _yaw);

        UpdateCameraVectors();
    }

    void Camera::LookAt(const glm::vec3 target)
    {
        const auto direction = glm::normalize(target - _position);

        _pitch = glm::degrees(std::asin(direction.y));
        _yaw = glm::degrees(std::atan2(direction.z, direction.x));

        UpdateCameraVectors();
    }

    void Camera::Zoom(const float offset)
    {
        _zoom -= offset;
        if (_zoom < 1.0f)
        {
            _zoom = 1.0f;
        }
        else if (_zoom > 45.0f)
        {
            _zoom = 45.0f;
        }
    }

    void Camera::UpdateScreenSize(const float screenWidth, const float screenHeight)
    {
        _screenWidth = screenWidth;
        _screenHeight = screenHeight;
    }

    void Camera::UpdateProjectionType(const ProjectionType projectionType)
    {
        _projectionType = projectionType;
    }

    void Camera::CreateFlashlight(SpotLightsFactory& spotLightsFactory)
    {
        _flashlight = std::make_unique<SpotLightSource>(spotLightsFactory.CreateSpotlight(_position, _front, 12.5f, 17.5f));
        UpdateUseFlashlight(false);
    }

    void Camera::SetFlashlightUniforms(const std::shared_ptr<Shader>& lightingPassShader) const
    {
        if (_flashlight == nullptr)
        {
            return;
        }
        _flashlight->SetUniforms(lightingPassShader);
    }

    void Camera::UpdateUseFlashlight(const bool useFlashlight) const
    {
        if (_flashlight == nullptr)
        {
            return;
        }
        if (useFlashlight && _projectionType == ProjectionType::PERSPECTIVE)
        {
            _flashlight->Activate();
        }
        else
        {
            _flashlight->Deactivate();
        }
    }

    void Camera::UpdateCameraVectors()
    {
        glm::vec3 front;
        front.x = static_cast<float>(cos(glm::radians(_yaw)) * static_cast<float>(cos(glm::radians(_pitch))));
        front.y = static_cast<float>(sin(glm::radians(_pitch)));
        front.z = static_cast<float>(sin(glm::radians(_yaw))) * static_cast<float>(cos(glm::radians(_pitch)));
        _front = glm::normalize(front);
        _right = glm::normalize(glm::cross(_front, _worldUp));
        _up = glm::normalize(glm::cross(_right, _front));
        spdlog::info("Updating camera vectors");
        if (_flashlight != nullptr)
        {
            _flashlight->UpdateDirection(_front);
        }
    }
} // Renderer3D