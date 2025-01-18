//
// Created by Kacper Trzciński on 18.01.2025.
//

#include <glm/gtc/matrix_transform.hpp>

#include "entity.h"

namespace Renderer3D {
    Entity::Entity(const fs::path& modelPath, bool flipTextures, const glm::vec3 position, const float rotationX, const float rotationY, const float rotationZ, const glm::vec3 scale) : _position(position), _rotationX(rotationX), _rotationY(rotationY), _rotationZ(rotationZ), _scale(scale)
    {
        _model = std::make_shared<Model>(modelPath, flipTextures);
    }

    Entity::Entity(const std::shared_ptr<Model>& model, const glm::vec3 position, const float rotationX, const float rotationY, const float rotationZ, const glm::vec3 scale) : _model(model) ,_position(position), _rotationX(rotationX), _rotationY(rotationY), _rotationZ(rotationZ), _scale(scale)
    {
    }

    glm::vec3 Entity::GetPosition() const
    {
        return _position;
    }

    void Entity::UpdatePosition(const glm::vec3 position)
    {
        _position = position;
    }

    float Entity::GetRotationX() const
    {
        return _rotationX;
    }

    void Entity::UpdateRotationX(const float rotationX)
    {
        _rotationX = rotationX;
    }

    float Entity::GetRotationY() const
    {
        return _rotationY;
    }

    void Entity::UpdateRotationY(const float rotationY)
    {
        _rotationY = rotationY;
    }

    float Entity::GetRotationZ() const
    {
        return _rotationZ;
    }

    void Entity::UpdateRotationZ(const float rotationZ)
    {
        _rotationZ = rotationZ;
    }

    glm::vec3 Entity::GetScale() const
    {
        return _scale;
    }

    void Entity::UpdateScale(const glm::vec3 scale)
    {
        _scale = scale;
    }

    glm::mat4 Entity::GetModelMatrix() const
    {
        auto modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, _position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(_rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(_rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(_rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, _scale);
        return modelMatrix;
    }

    void Entity::Draw(const std::shared_ptr<Shader>& shader) const
    {
        shader->SetUniform("model", GetModelMatrix());
        _model->Draw(shader);
    }
} // Renderer3D