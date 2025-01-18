//
// Created by Kacper Trzciński on 18.01.2025.
//

#ifndef ENTITY_H
#define ENTITY_H

#include <memory>

#include "model.h"

namespace fs = std::filesystem;

namespace Renderer3D {

    class Entity {
    public:
        explicit Entity(const fs::path& modelPath, bool flipTextures = false, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float rotationX = 0.0f, float rotationY = 0.0f, float rotationZ = 0.0f, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
        explicit Entity(const std::shared_ptr<Model>& model, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float rotationX = 0.0f, float rotationY = 0.0f, float rotationZ = 0.0f, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
        [[nodiscard]] glm::vec3 GetPosition() const;
        void UpdatePosition(glm::vec3 position);
        [[nodiscard]] float GetRotationX() const;
        void UpdateRotationX(float rotationX);
        [[nodiscard]] float GetRotationY() const;
        void UpdateRotationY(float rotationY);
        [[nodiscard]] float GetRotationZ() const;
        void UpdateRotationZ(float rotationZ);
        [[nodiscard]] glm::vec3 GetScale() const;
        void UpdateScale(glm::vec3 scale);
        [[nodiscard]] glm::mat4 GetModelMatrix() const;
        void Draw(const std::shared_ptr<Shader>& shader) const;
    private:
        std::shared_ptr<Model> _model;
        glm::vec3 _position;
        float _rotationX;
        float _rotationY;
        float _rotationZ;
        glm::vec3 _scale;
    };

} // Renderer3D

#endif //ENTITY_H
