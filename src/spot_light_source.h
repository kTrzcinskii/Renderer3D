//
// Created by Kacper Trzciński on 22.01.2025.
//

#ifndef SPOT_LIGHT_SOURCE_H
#define SPOT_LIGHT_SOURCE_H

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "shader.h"

namespace Renderer3D {

    class SpotLightSource {
    public:
        SpotLightSource(size_t id, glm::vec3 position, glm::vec3 direction, float cutOff, float outerCutOff, float linear = SpotLightSource::DEFAULT_LINEAR, float quadratic = SpotLightSource::DEFAULT_QUADRATIC);
        void UpdatePosition(glm::vec3 position);
        void UpdateDirection(glm::vec3 direction);
        void Activate();
        void Deactivate();
        void SetUniforms(const std::shared_ptr<Shader>& shader) const;
    private:
        glm::vec3 _position;
        glm::vec3 _direction;
        // NOTE: cutOff and outerCutOff are both angles in degrees
        // cutOff < outerCutOff must always be true
        float _cutOff;
        float _outerCutOff;
        float _linear;
        float _quadratic;
        bool _shouldUse = true;
        size_t _ID;

        // Const
        static constexpr float DEFAULT_LINEAR = 0.09f;
        static constexpr float DEFAULT_QUADRATIC = 0.032;
    };

} // Renderer3D

#endif //SPOT_LIGHT_SOURCE_H
