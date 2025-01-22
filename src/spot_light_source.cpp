//
// Created by Kacper Trzciński on 22.01.2025.
//

#include <format>

#include "spot_light_source.h"

#include "spdlog/spdlog.h"

namespace Renderer3D {
    SpotLightSource::SpotLightSource(const size_t id, const glm::vec3 position, const glm::vec3 direction, const float cutOff, const float outerCutOff, const float linear, const float quadratic) : _position(position), _direction(direction), _cutOff(glm::cos(glm::radians(cutOff))), _outerCutOff(glm::cos(glm::radians(outerCutOff))), _linear(linear), _quadratic(quadratic), _ID(id)
    {
        if (cutOff >= outerCutOff)
        {
            spdlog::error("SpotLightSource: cutOff out of range ({} - {})", cutOff, outerCutOff);
        }
    }

    void SpotLightSource::UpdatePosition(const glm::vec3 position)
    {
        _position = position;
    }

    void SpotLightSource::UpdateDirection(const glm::vec3 direction)
    {
        _direction = direction;
    }

    void SpotLightSource::Activate()
    {
        _shouldUse = true;
    }

    void SpotLightSource::Deactivate()
    {
        _shouldUse = false;
    }

    void SpotLightSource::SetUniforms(const std::shared_ptr<Shader>& shader) const
    {
        const auto name = std::format("spotLights[{}]", _ID);
        shader->SetUniform(std::format("{}.use", name), _shouldUse);
        if (!_shouldUse)
        {
            return;
        }
        shader->SetUniform(std::format("{}.position", name), _position);
        shader->SetUniform(std::format("{}.direction", name), _direction);
        shader->SetUniform(std::format("{}.cutOff", name), _cutOff);
        shader->SetUniform(std::format("{}.outerCutOff", name), _outerCutOff);
        shader->SetUniform(std::format("{}.linear", name), _linear);
        shader->SetUniform(std::format("{}.quadratic", name), _quadratic);
    }
} // Renderer3D