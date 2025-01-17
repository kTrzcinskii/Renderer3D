//
// Created by Kacper Trzciński on 17.01.2025.
//

#ifndef POINT_LIGHT_SOURCE_H
#define POINT_LIGHT_SOURCE_H

#include <memory>
#include <glm/glm.hpp>

#include "shader.h"

namespace Renderer3D {

    class PointLightSource {
    public:
        PointLightSource(glm::vec3 position, glm::vec3 color, float linear = PointLightSource::DEFAULT_LINEAR, float quadratic = PointLightSource::DEFAULT_QUADRATIC);
        void SetUniforms(const std::shared_ptr<Shader>& shader, size_t idx) const;
        [[nodiscard]] glm::vec3 GetPosition() const;
        [[nodiscard]] glm::vec3 GetColor() const;
    private:
        glm::vec3 _position;
        glm::vec3 _color;
        float _linear;
        float _quadratic;
        float _radius;

        // Consts
        static constexpr float DEFAULT_LINEAR = 0.7f;
        static constexpr float DEFAULT_QUADRATIC = 1.8f;
    };

} // Renderer3D

#endif //POINT_LIGHT_SOURCE_H
