//
// Created by Kacper Trzciński on 17.01.2025.
//

#include <format>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "point_light_source.h"

namespace Renderer3D {
    PointLightSource::PointLightSource(const glm::vec3 position, const glm::vec3 color, const float linear, const float quadratic)
    {
        _position = position;
        _color = color;
        _linear = linear;
        _quadratic = quadratic;
        const auto maxBrightness = std::fmaxf(std::fmaxf(_color.r, _color.g), _color.b);
        _radius = (-_linear + std::sqrt(_linear * _linear - 4 * _quadratic * (1.0f - (256.0f / 5.0f) * maxBrightness))) / (2.0f * _quadratic);
    }

    void PointLightSource::SetUniforms(const std::shared_ptr<Shader>& shader, const size_t idx) const
    {
        const auto name = std::format("pointLights[{}]", idx);
        shader->SetUniform(std::format("{}.position", name), _position);
        shader->SetUniform(std::format("{}.color", name), _color);
        shader->SetUniform(std::format("{}.linear", name), _linear);
        shader->SetUniform(std::format("{}.quadratic", name), _quadratic);
        shader->SetUniform(std::format("{}.radius", name), _radius);
    }

    glm::vec3 PointLightSource::GetPosition() const
    {
        return _position;
    }

    glm::vec3 PointLightSource::GetColor() const
    {
        return _color;
    }

    PointLightSource PointLightSource::GenerateRandom(const float minX, const float maxX, const float minY, const float maxY, const float minZ, const float maxZ)
    {
        // Random offsets
        const auto xPos = rand() / static_cast<float>(RAND_MAX) * (maxX - minX) + minX;
        const auto yPos = rand() / static_cast<float>(RAND_MAX) * (maxY - minY) + minY;
        const auto zPos = rand() / static_cast<float>(RAND_MAX) * (maxZ - minZ) + minZ;
        const auto position = glm::vec3(xPos, yPos, zPos);

        // Random color
        const auto rColor = static_cast<float>(rand() % 100 / 200.0f + 0.5); // between 0.5 and 1.
        const auto gColor = static_cast<float>(rand() % 100 / 200.0f + 0.5); // between 0.5 and 1.
        const auto bColor = static_cast<float>(rand() % 100 / 200.0f + 0.5); // between 0.5 and 1.
        const auto color = glm::vec3(rColor, gColor, bColor);

        return PointLightSource(position, color);
    }

} // Renderer3D